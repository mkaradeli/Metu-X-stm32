/*
 * ring_buffer.h
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 *
 *  Single-core (STM32H753) revision.
 *
 *  Multi-producer, single-consumer byte ring drained over UART by DMA.
 *  Producer side (rb_write()/rb_push()) is safe to call from the main loop
 *  and from any ISR, at any NVIC priority, concurrently -- the read-modify-
 *  write of `head` is wrapped in a critical section for exactly that reason.
 *  Consumer is the DMA engine, advanced from the TX-complete interrupt.
 *
 *  Storage belongs in SRAM3 (0x30040000, D2 domain) so DMA1/DMA2 reach it
 *  without crossing an inter-domain bus. Mark that region non-cacheable in
 *  the MPU and no cache maintenance is needed anywhere in this module.
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32h7xx_hal.h"

#define RB_SIZE  16384u                  /* must be power of 2 */
#define RB_MASK  (RB_SIZE - 1u)

/* Upper bound on one DMA burst. Bounds the interval between TX-complete
 * interrupts and the amount of buffer parked mid-flight. */
#ifndef RB_TX_CHUNK_MAX
#define RB_TX_CHUNK_MAX 512u
#endif

/* Only needed if the storage region is left cacheable. Keep at 0 and give
 * SRAM3 an MPU region with TEX=1, C=0, B=0. */
#ifndef RB_TX_CACHE_MAINTENANCE
#define RB_TX_CACHE_MAINTENANCE 0
#endif

/* Ordering between producer and the TX-complete ISR is guaranteed by the
 * core itself; only the compiler needs restraining. */
#define RB_COMPILER_BARRIER()  __asm volatile ("" ::: "memory")

/* volatile applies to the indices, which cross the thread/ISR boundary -
 * NOT to the payload, so bulk copies stay as memcpy. */
typedef struct {
	volatile uint32_t head;         /* producer writes, ISR reads          */
	volatile uint32_t tail;         /* ISR writes, producer reads          */
	volatile uint32_t dropped;      /* bytes lost to overflow              */
	volatile uint32_t tx_errors;
	uint32_t _pad[4];               /* payload starts on a 32-byte line    */
	uint8_t  buffer[RB_SIZE];
} __attribute__((aligned(32))) rb_t;

/* --- producer side ---------------------------------------------------- */

/* Bytes that can be accepted right now (one slot is always reserved so that
 * head == tail unambiguously means empty). */
static inline uint32_t rb_free(const rb_t *rb)
{
	return (rb->tail - rb->head - 1u) & RB_MASK;
}

static inline uint32_t rb_count(const rb_t *rb)
{
	return (rb->head - rb->tail) & RB_MASK;
}

static inline bool rb_push(rb_t *rb, uint8_t c)
{
	/* Same read-h/write-h race as rb_write() -- see its comment. */
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	uint32_t h    = rb->head;
	uint32_t next = (h + 1u) & RB_MASK;

	bool ok = (next != rb->tail);
	if (ok) {
		rb->buffer[h] = c;
		RB_COMPILER_BARRIER();          /* byte stored before index update */
		rb->head = next;
	} else {
		rb->dropped++;                  /* full */
	}

	__set_PRIMASK(primask);
	return ok;
}

/* Bulk insert. Copies as much as fits, counts the remainder as dropped,
 * returns bytes accepted. Two memcpys at worst (one either side of wrap). */
size_t rb_write(rb_t *rb, const void *src, size_t n);

void   rb_init(rb_t *rb);
size_t rb_dropped(const rb_t *rb);

/* --- DMA drain path --------------------------------------------------- */

/* Bind buffer and UART. Call once, after MX_USARTx_UART_Init(). */
void   rb_tx_init(rb_t *rb, UART_HandleTypeDef *huart);

/* Non-blocking: starts a DMA transfer of the largest contiguous run if the
 * link is idle and bytes are pending. Call from the main loop. Once primed,
 * the TX-complete ISR chains the next chunk on its own. */
void   rb_flush(void);

bool   rb_tx_busy(void);

/* Hook into HAL_UART_TxCpltCallback / HAL_UART_ErrorCallback. Definitions
 * are provided in ring_buffer.c unless RB_NO_UART_CALLBACKS is defined. */
void   rb_tx_complete_isr(void);
void   rb_tx_error_isr(void);

/* Spin until empty and idle. Orderly shutdown only - needs interrupts on. */
void   rb_drain_blocking(uint32_t timeout_ms);

/* Interrupt-free polled drain straight at the UART data register. For
 * HardFault_Handler and friends, where the DMA path cannot complete. */
void   rb_flush_panic(void);

extern rb_t common_print_buffer;
extern UART_HandleTypeDef *s_huart;

#ifdef __cplusplus
}
#endif

#endif /* INC_RING_BUFFER_H_ */
