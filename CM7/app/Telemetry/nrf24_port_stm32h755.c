/**
 * @file    nrf24_port_stm32h755.c
 * @brief   Bare-metal (CMSIS register-level, no HAL) port for the MetuX flight
 *          computer, STM32H755ZITx, CM7 core.
 *
 * Pinout, per MetuX.ioc (TLMTR_* labels) - this is the real board, not a
 * generic Nucleo dev-board default:
 *
 *   SPI4_SCK   PE2   AF5
 *   SPI4_MISO  PE5   AF5
 *   SPI4_MOSI  PE6   AF5
 *   CSN        PE4   (TLMTR_CSN)
 *   CE         PE3   (TLMTR_CE)
 *   IRQ        PD7   (TLMTR_IRQ)
 *
 * All six pins are CM7-owned in the .ioc (PinAttribute/ContextOwner =
 * CortexM7) - do not call this port's init from the CM4 side.
 *
 * SPI4's kernel clock (pll3_q_ck, ~4 MHz per RCC.SPI45Freq_Value in the
 * .ioc) and its RCC_D2CCIP1R_SPI45SEL mux are brought up by CubeMX-generated
 * code: MX_SPI4_Init() -> HAL_SPI_MspInit() in Core/Src/spi.c, which also
 * configures and locks PLL3. That bring-up is NOT duplicated here - it is
 * nontrivial (PLL3 M/N/P/Q/R + VCO range + lock wait) and already correct.
 * main() MUST keep calling MX_SPI4_Init() before app init runs this port's
 * nrf24_port_init(); this file only takes over the SPI4 register content
 * (CR1/CFG1/CFG2, full-overwrite so whatever HAL left behind - including a
 * stray 4-bit DataSize the .ioc doesn't track - gets replaced) and CE/CSN/
 * IRQ/SPI pin control from there on.
 *
 * RCC->AHB4ENR / RCC->APB2ENR are single, shared registers across both
 * CM7 and CM4 on H755. The read-modify-write here is safe only because
 * GPIOD/GPIOE and SPI4 are CM7-owned peripherals that CM4 never touches;
 * do not reuse this pattern for a peripheral shared with the CM4 side
 * without an HSEM guard.
 *
 * Transfers are polled PIO, not DMA, so there is no D-cache maintenance to
 * do here (33 bytes @ <=2 MHz is well under a millisecond).
 */

#include "stm32h7xx.h"
#include "nrf24_port.h"
#include "stdbool.h"

/* ------------------------------------------------------------------ config */
#define NRF24_SPI                 SPI4
#define NRF24_SPI_KERNEL_HZ       4000000UL     /* pll3_q_ck feeding SPI45SEL, see .ioc */
#define NRF24_SPI_MAX_HZ          8000000UL     /* nRF24L01+ spec limit is 10 MHz */

#define NRF24_SCK_PORT            GPIOE
#define NRF24_SCK_PIN             2u
#define NRF24_MISO_PORT           GPIOE
#define NRF24_MISO_PIN            5u
#define NRF24_MOSI_PORT           GPIOE
#define NRF24_MOSI_PIN            6u
#define NRF24_SPI_AF              5u

#define NRF24_CSN_PORT            GPIOE
#define NRF24_CSN_PIN             4u
#define NRF24_CE_PORT             GPIOE
#define NRF24_CE_PIN              3u
#define NRF24_IRQ_PORT            GPIOD
#define NRF24_IRQ_PIN             7u

/* Set to 1 if your application already owns DWT (e.g. an existing CPU profiler);
 * then nrf24_port_micros() only reads CYCCNT and never reconfigures it.        */
#ifndef NRF24_PORT_DWT_EXTERNALLY_OWNED
#define NRF24_PORT_DWT_EXTERNALLY_OWNED  0
#endif

/* --------------------------------------------------------------- utilities */
#define GPIO_MODE_OUT(p, n)   do { (p)->MODER = ((p)->MODER & ~(3u << (2u*(n)))) | (1u << (2u*(n))); } while (0)
#define GPIO_MODE_AF(p, n)    do { (p)->MODER = ((p)->MODER & ~(3u << (2u*(n)))) | (2u << (2u*(n))); } while (0)
#define GPIO_MODE_IN(p, n)    do { (p)->MODER &= ~(3u << (2u*(n))); } while (0)
#define GPIO_SPEED_VHI(p, n)  do { (p)->OSPEEDR |= (3u << (2u*(n))); } while (0)
#define GPIO_PULLUP(p, n)     do { (p)->PUPDR = ((p)->PUPDR & ~(3u << (2u*(n)))) | (1u << (2u*(n))); } while (0)
#define GPIO_SET_AF(p, n, af) do { (p)->AFR[(n) >> 3] = ((p)->AFR[(n) >> 3] & ~(0xFu << (4u*((n) & 7u)))) \
                                                        | ((uint32_t)(af) << (4u*((n) & 7u))); } while (0)

static uint32_t s_cycles_per_us = 400u;

static uint32_t spi_mbr_for(uint32_t f_ker, uint32_t f_max)
{
    /* f_sck = f_ker / 2^(MBR+1) ; MBR in 0..7 -> /2 .. /256 */
    uint32_t mbr = 0u;
    while ((mbr < 7u) && ((f_ker >> (mbr + 1u)) > f_max)) {
        mbr++;
    }
    return mbr;
}

/* ------------------------------------------------------------------- init */
void nrf24_port_init(void)
{
    /* GPIOE (SPI4 + CE/CSN), GPIOD (IRQ). CubeMX's MX_GPIO_Init() already
     * enables these, but this port is meant to stand on its own. */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN | RCC_AHB4ENR_GPIOEEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
    (void)RCC->APB2ENR;

    /* SPI45SEL (pll3_q_ck) + PLL3 itself are owned by MX_SPI4_Init() /
     * HAL_SPI_MspInit() in Core/Src/spi.c - deliberately not touched here.
     * See file header. */

    /* CSN high before anything else so the radio never sees a partial frame */
    NRF24_CSN_PORT->BSRR = (1u << NRF24_CSN_PIN);
    GPIO_MODE_OUT(NRF24_CSN_PORT, NRF24_CSN_PIN);
    GPIO_SPEED_VHI(NRF24_CSN_PORT, NRF24_CSN_PIN);

    NRF24_CE_PORT->BSRR = (1u << (NRF24_CE_PIN + 16u));   /* CE low */
    GPIO_MODE_OUT(NRF24_CE_PORT, NRF24_CE_PIN);
    GPIO_SPEED_VHI(NRF24_CE_PORT, NRF24_CE_PIN);

    GPIO_MODE_IN(NRF24_IRQ_PORT, NRF24_IRQ_PIN);
    GPIO_PULLUP(NRF24_IRQ_PORT, NRF24_IRQ_PIN);           /* IRQ is open-drain-ish, active low */

    GPIO_SET_AF(NRF24_SCK_PORT,  NRF24_SCK_PIN,  NRF24_SPI_AF);
    GPIO_SET_AF(NRF24_MISO_PORT, NRF24_MISO_PIN, NRF24_SPI_AF);
    GPIO_SET_AF(NRF24_MOSI_PORT, NRF24_MOSI_PIN, NRF24_SPI_AF);
    GPIO_SPEED_VHI(NRF24_SCK_PORT,  NRF24_SCK_PIN);
    GPIO_SPEED_VHI(NRF24_MOSI_PORT, NRF24_MOSI_PIN);
    GPIO_MODE_AF(NRF24_SCK_PORT,  NRF24_SCK_PIN);
    GPIO_MODE_AF(NRF24_MISO_PORT, NRF24_MISO_PIN);
    GPIO_MODE_AF(NRF24_MOSI_PORT, NRF24_MOSI_PIN);

    /* --- SPI4: master, 8-bit, MSB first, mode 0, software NSS ---
     * Full overwrite of CR1/CFG1/CFG2: whatever CubeMX's MX_SPI4_Init()
     * left in these registers (including its 4-bit DataSize) is replaced. */
    NRF24_SPI->CR1  = 0u;                                  /* SPE = 0 while configuring */
    NRF24_SPI->CFG1 = (spi_mbr_for(NRF24_SPI_KERNEL_HZ, NRF24_SPI_MAX_HZ) << SPI_CFG1_MBR_Pos)
                    | (7u << SPI_CFG1_DSIZE_Pos)           /* DSIZE = 8-1 */
                    | (0u << SPI_CFG1_FTHLV_Pos);          /* FIFO threshold = 1 data */
    NRF24_SPI->CFG2 = SPI_CFG2_MASTER                      /* CPOL = CPHA = 0, MSB first */
                    | SPI_CFG2_SSM
                    | SPI_CFG2_AFCNTR;                     /* keep AF driven while SPE = 0 */
    NRF24_SPI->CR1  = SPI_CR1_SSI;                         /* SSM=1 needs SSI=1, else MODF */

    /* Clear every sticky fault flag (MODF/OVR/CRCE/UDR/TIFRE/...) before the
     * first SPE=1. A warm ST-LINK reset does not necessarily clear these on
     * its own; if one is already latched from before this function ran,
     * setting SPE re-trips it immediately and hardware auto-disables SPE
     * before a single bit is clocked - the transfer loop below then spins
     * forever waiting on a byte that will never arrive. */
    NRF24_SPI->IFCR = 0xFFFFFFFFu;

#if (NRF24_PORT_DWT_EXTERNALLY_OWNED == 0)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    *(volatile uint32_t *)0xE0001FB0u = 0xC5ACCE55u;       /* DWT_LAR unlock (Cortex-M7) */
    DWT->CYCCNT = 0u;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
#endif
    s_cycles_per_us = SystemCoreClock / 1000000u;
    if (s_cycles_per_us == 0u) {
        s_cycles_per_us = 1u;
    }
}

/* ------------------------------------------------------------ pin control */
void nrf24_port_csn(int level)
{
    NRF24_CSN_PORT->BSRR = level ? (1u << NRF24_CSN_PIN)
                                 : (1u << (NRF24_CSN_PIN + 16u));
}

void nrf24_port_ce(int level)
{
    NRF24_CE_PORT->BSRR = level ? (1u << NRF24_CE_PIN)
                                : (1u << (NRF24_CE_PIN + 16u));
}

int nrf24_port_irq_level(void)
{
    return (NRF24_IRQ_PORT->IDR & (1u << NRF24_IRQ_PIN)) ? 1 : 0;
}

/* ------------------------------------------------------------- SPI burst */
/* A wedged SPI4 (fault-disabled SPE, dead kernel clock, ...) must never turn
 * into an unbounded hang here - this runs inside a flight computer's radio
 * task, and nrf24.h's own contract promises callers it never blocks longer
 * than one burst. On timeout we abort, clear the fault latches, and disable
 * SPE so the next call starts clean; the caller finds out indirectly, via
 * nrf24.c's own register-readback / TX-timeout checks (this function has no
 * return value in the port contract, so it cannot report failure directly). */
#define NRF24_SPI_XFER_TIMEOUT_US   1000u

void nrf24_port_spi_xfer(const uint8_t *tx, uint8_t *rx, size_t n)
{
    volatile uint8_t *const txdr = (volatile uint8_t *)&NRF24_SPI->TXDR;
    volatile uint8_t *const rxdr = (volatile uint8_t *)&NRF24_SPI->RXDR;
    const uint32_t deadline = nrf24_port_micros() + NRF24_SPI_XFER_TIMEOUT_US;
    size_t i;
    bool timed_out = false;

    if (n == 0u) {
        return;
    }

    NRF24_SPI->CR2 = (uint32_t)n;              /* TSIZE: exact byte count */
    NRF24_SPI->CR1 |= SPI_CR1_SPE;
    NRF24_SPI->CR1 |= SPI_CR1_CSTART;

    for (i = 0u; (i < n) && !timed_out; i++) {
        while ((NRF24_SPI->SR & SPI_SR_TXP) == 0u) {
            if ((int32_t)(nrf24_port_micros() - deadline) >= 0) { timed_out = true; break; }
        }
        if (timed_out) { break; }
        *txdr = (tx != NULL) ? tx[i] : 0xFFu;  /* byte-wide store: no FIFO packing */
        while ((NRF24_SPI->SR & SPI_SR_RXP) == 0u) {
            if ((int32_t)(nrf24_port_micros() - deadline) >= 0) { timed_out = true; break; }
        }
        if (timed_out) { break; }
        {
            uint8_t d = *rxdr;
            if (rx != NULL) {
                rx[i] = d;
            }
        }
    }

    if (!timed_out) {
        while ((NRF24_SPI->SR & SPI_SR_EOT) == 0u) {
            if ((int32_t)(nrf24_port_micros() - deadline) >= 0) { timed_out = true; break; }
        }
    }

    NRF24_SPI->IFCR = 0xFFFFFFFFu;             /* clear EOT/TXTF and any latched fault */
    NRF24_SPI->CR1 &= ~SPI_CR1_SPE;            /* clears the FIFOs for the next frame */
}

/* --------------------------------------------------------------- timebase */
uint32_t nrf24_port_micros(void)
{
    return DWT->CYCCNT / s_cycles_per_us;
}

void nrf24_port_delay_us(uint32_t us)
{
    const uint32_t start = DWT->CYCCNT;
    const uint32_t ticks = us * s_cycles_per_us;
    while ((DWT->CYCCNT - start) < ticks) { }
}
