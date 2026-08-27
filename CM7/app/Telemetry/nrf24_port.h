/**
 * @file    nrf24_port.h
 * @brief   Hardware glue required by nrf24.c. Implement these 6 functions for any MCU.
 *
 * A bare-metal STM32H755 (CM7) implementation is provided in nrf24_port_stm32h755.c.
 * The driver never touches a vendor HAL; everything below is the entire contract.
 */
#ifndef NRF24_PORT_H
#define NRF24_PORT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Bring up SPI + CSN/CE/IRQ GPIOs. CSN idles high, CE idles low. */
void     nrf24_port_init(void);

/** CSN (active low chip select). level: 0 = asserted, 1 = released. */
void     nrf24_port_csn(int level);

/** CE (radio enable). level: 1 = TX burst / RX listening. */
void     nrf24_port_ce(int level);

/**
 * Full-duplex SPI burst, MSB first, Mode 0 (CPOL=0, CPHA=0), <= 8 MHz.
 * tx may be NULL (sends 0xFF), rx may be NULL (discards). n <= 33.
 * Must be synchronous: returns only when the last bit has been clocked out.
 */
void     nrf24_port_spi_xfer(const uint8_t *tx, uint8_t *rx, size_t n);

/** Free-running microsecond counter (wraps at 2^32; only differences are used). */
uint32_t nrf24_port_micros(void);

/** Busy-wait, us >= 1. */
void     nrf24_port_delay_us(uint32_t us);

/** Optional: current IRQ pin level (1 = idle, 0 = pending). Return 1 if unused. */
int      nrf24_port_irq_level(void);

#ifdef __cplusplus
}
#endif

#endif /* NRF24_PORT_H */
