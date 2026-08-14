/*
 * ring_buffer.c
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 *
 *  Single-core (STM32H753) revision.
 */

#include "ring_buffer.h"

/* Storage in SRAM3 (D2). The linker script must place .sram3 at 0x30040000
 * and the SRAM3 clock must be enabled (RCC_AHB2ENR, D2SRAM3EN) before use. */
__attribute__((section(".RAM_D2_Section"), used, aligned(32)))
rb_t common_print_buffer;

/* --- drain state ------------------------------------------------------ */

#define RB_TX_CLAIMED  0xFFFFFFFFu      /* claimed, length not yet decided */

UART_HandleTypeDef *s_huart = NULL;
static rb_t               *s_rb    = NULL;
static volatile uint32_t   s_tx_len = 0u;   /* 0 = idle, else bytes in flight */

/* --- producer side ---------------------------------------------------- */

void rb_init(rb_t *rb)
{
	rb->head      = 0u;
	rb->tail      = 0u;
	rb->dropped   = 0u;
	rb->tx_errors = 0u;
}

size_t rb_write(rb_t *rb, const void *src, size_t n)
{
	const uint8_t *p = (const uint8_t *)src;

	uint32_t h     = rb->head;          /* only this context writes head   */
	uint32_t space = rb_free(rb);       /* single volatile read of tail    */

	if (n > space) {
		rb->dropped += (uint32_t)(n - space);
		n = space;
	}
	if (n == 0u) {
		return 0u;
	}

	uint32_t first = RB_SIZE - h;       /* bytes before the wrap point     */
	if (first > n) {
		first = (uint32_t)n;
	}

	memcpy(&rb->buffer[h], p, first);
	if (n > first) {
		memcpy(&rb->buffer[0], p + first, n - first);
	}

	RB_COMPILER_BARRIER();              /* payload stored before index     */
	rb->head = (uint32_t)(h + n) & RB_MASK;
	return n;
}

size_t rb_dropped(const rb_t *rb)
{
	return rb->dropped;
}

/* --- DMA drain path --------------------------------------------------- */

void rb_tx_init(rb_t *rb, UART_HandleTypeDef *huart)
{
	s_rb     = rb;
	s_huart  = huart;
	s_tx_len = 0u;
}

bool rb_tx_busy(void)
{
	return (s_tx_len != 0u);
}

/* rb_flush() runs from the main loop and is also chained from the
 * TX-complete ISR, so claiming the transmitter must not be interruptible. */
static inline bool rb_tx_claim(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	bool got = (s_tx_len == 0u);
	if (got) {
		s_tx_len = RB_TX_CLAIMED;
	}

	__set_PRIMASK(primask);
	return got;
}

void rb_flush(void)
{
	rb_t *rb = s_rb;

	if ((rb == NULL) || (s_huart == NULL)) {
		return;
	}
	if (!rb_tx_claim()) {
		return;                         /* transfer already in flight      */
	}

	uint32_t t = rb->tail;
	uint32_t h = rb->head;

	if (t == h) {                       /* empty - release the claim       */
		s_tx_len = 0u;
		return;
	}

	/* DMA needs a linear block, so never cross the wrap point; a wrapped
	 * payload simply becomes two transfers. */
	uint32_t len = (h > t) ? (h - t) : (RB_SIZE - t);
	if (len > RB_TX_CHUNK_MAX) {
		len = RB_TX_CHUNK_MAX;
	}

	s_tx_len = len;

#if RB_TX_CACHE_MAINTENANCE
	uint32_t addr  = (uint32_t)(uintptr_t)&rb->buffer[t];
	uint32_t start = addr & ~0x1Fu;
	SCB_CleanDCache_by_Addr((uint32_t *)start, (int32_t)((addr - start) + len));
#endif

	/* The DMA is a separate observer: order the payload stores against the
	 * upcoming Device-memory writes that arm the transfer. */
	__DMB();

	if (HAL_UART_Transmit_DMA(s_huart, &rb->buffer[t], (uint16_t)len) != HAL_OK) {
		rb->tx_errors++;
		s_tx_len = 0u;                  /* tail untouched; retry next call */
	}
}

/* The tail advances only here. While a transfer is in flight the tail stays
 * put, so the producer sees the in-flight region as occupied and drops
 * rather than overwriting bytes the DMA is still fetching. */
void rb_tx_complete_isr(void)
{
	uint32_t n = s_tx_len;

	if ((n == 0u) || (n == RB_TX_CLAIMED)) {
		return;
	}

	s_rb->tail = (s_rb->tail + n) & RB_MASK;
	RB_COMPILER_BARRIER();
	s_tx_len = 0u;

	rb_flush();                         /* chain into the next chunk       */
}

void rb_tx_error_isr(void)
{
	if (s_rb != NULL) {
		s_rb->tx_errors++;
	}
	s_tx_len = 0u;                      /* tail untouched: chunk is resent */
}

void rb_drain_blocking(uint32_t timeout_ms)
{
	uint32_t t0 = HAL_GetTick();

	while ((s_rb != NULL) && ((rb_count(s_rb) != 0u) || rb_tx_busy())) {
		rb_flush();
		if ((HAL_GetTick() - t0) > timeout_ms) {
			return;
		}
	}
}

void rb_flush_panic(void)
{
	rb_t *rb = s_rb;

	if ((rb == NULL) || (s_huart == NULL)) {
		return;
	}

	USART_TypeDef *u = s_huart->Instance;

	/* Drop the DMA request line and drive the data register directly - no
	 * interrupts, no HAL state machine. */
	u->CR3 &= ~USART_CR3_DMAT;

	while (rb->tail != rb->head) {
		while ((u->ISR & USART_ISR_TXE_TXFNF) == 0u) {
			/* wait for the TX FIFO to accept a byte */
		}
		u->TDR = rb->buffer[rb->tail];
		rb->tail = (rb->tail + 1u) & RB_MASK;
	}

	while ((u->ISR & USART_ISR_TC) == 0u) {
		/* wait for the last frame to leave the shift register */
	}
}

/* --- HAL hooks --------------------------------------------------------
 * Delete this block and call the two *_isr() functions from your own
 * callbacks if they are already defined elsewhere (-DRB_NO_UART_CALLBACKS). */
#ifndef RB_NO_UART_CALLBACKS

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if (huart == s_huart) {
//		rb_tx_complete_isr();
//	}
//}

//void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
//{
//	if (huart == s_huart) {
//		rb_tx_error_isr();
//	}
//}

#endif /* RB_NO_UART_CALLBACKS */
