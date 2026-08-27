/**
 * @file    nrf24.h
 * @brief   Bare-metal nRF24L01+ / EBYTE E01-ML01DP5 driver.
 *
 * Design rules:
 *  - No dynamic allocation, no HAL, no blocking longer than one SPI burst in the
 *    hot path. nrf24_tx_service() is a non-blocking state machine so it can be
 *    called from a control loop without ever stalling it for a retransmit cycle.
 *  - One instance. Not reentrant: call from a single context (or wrap the calls
 *    in a critical section if you service it from both loop and EXTI).
 */
#ifndef NRF24_H
#define NRF24_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NRF24_MAX_PAYLOAD    32u
#define NRF24_ADDR_MAX       5u

typedef enum {
    NRF24_OK = 0,
    NRF24_ERR_PARAM,
    NRF24_ERR_NO_RADIO,     /* register readback failed: check MISO/CSN/SCK/power */
    NRF24_ERR_FIFO_FULL,
    NRF24_ERR_TIMEOUT,
    NRF24_ERR_MAX_RT        /* ARC retries exhausted, packet dropped */
} nrf24_status_t;

typedef enum {
    NRF24_DR_250K = 0,      /* -104 dBm module sensitivity, longest range      */
    NRF24_DR_1M,            /* ~-95 dBm                                        */
    NRF24_DR_2M             /* ~-93 dBm, shortest air time                     */
} nrf24_datarate_t;

typedef enum {              /* chip output; the E01 PA adds a fixed ~+22 dB    */
    NRF24_PWR_M18 = 0,      /* module ~ +2 dBm                                  */
    NRF24_PWR_M12,          /* module ~ +8 dBm                                  */
    NRF24_PWR_M6,           /* module ~ +14 dBm                                 */
    NRF24_PWR_0             /* module ~ +20 dBm, ~115 mA TX                      */
} nrf24_power_t;

typedef enum {
    NRF24_TX_IDLE = 0,
    NRF24_TX_BUSY,
    NRF24_TX_DONE,
    NRF24_TX_FAILED
} nrf24_tx_state_t;

typedef struct {
    uint8_t          channel;          /* 0..125 -> 2400 + ch MHz                    */
    nrf24_datarate_t data_rate;
    nrf24_power_t    power;
    uint8_t          addr_width;       /* 3..5 bytes                                 */
    uint8_t          tx_addr[NRF24_ADDR_MAX];
    uint8_t          rx_addr_p0[NRF24_ADDR_MAX];  /* = tx_addr when auto-ack is on   */
    uint8_t          rx_addr_p1[NRF24_ADDR_MAX];
    uint8_t          payload_len;      /* 1..32 fixed, or 0 = dynamic payload length */
    bool             auto_ack;         /* per-packet override available in tx_push   */
    bool             ack_payload;      /* PRX->PTX piggyback data (needs auto_ack)   */
    uint8_t          arc;              /* auto retransmit count 0..15                */
    uint16_t         ard_us;           /* auto retransmit delay, 250..4000, step 250 */
    bool             crc16;            /* false = 1 byte CRC                         */
} nrf24_cfg_t;

typedef struct {
    uint32_t tx_ok;
    uint32_t tx_dropped;   /* MAX_RT events                        */
    uint32_t tx_timeout;
    uint32_t rx_ok;
    uint32_t retransmits;  /* accumulated ARC_CNT                  */
    uint32_t lost_packets; /* PLOS_CNT snapshot at last read       */
} nrf24_stats_t;

/* --------------------------------------------------------------------- API */

/** Fill cfg with a sane VTVL-telemetry default: ch 76, 1 Mbps, +20 dBm, 32 B,
 *  auto-ack with ARC=2 / ARD=500 us, ack payload enabled, 16-bit CRC. */
void           nrf24_default_cfg(nrf24_cfg_t *cfg);

/** Port init + full radio configuration + readback verification. */
nrf24_status_t nrf24_init(const nrf24_cfg_t *cfg);

nrf24_status_t nrf24_set_channel(uint8_t ch);
nrf24_status_t nrf24_set_datarate(nrf24_datarate_t dr);
nrf24_status_t nrf24_set_power(nrf24_power_t pwr);

void           nrf24_standby(void);      /* CE low, PWR_UP=1  (~26 uA)        */
void           nrf24_power_down(void);   /* PWR_UP=0          (~900 nA)       */
void           nrf24_start_rx(void);     /* PRIM_RX=1, CE high                */
void           nrf24_start_tx(void);     /* PRIM_RX=0, CE low until a payload */

/* ---- transmit (PTX) ---- */

/** Queue one payload into the 3-deep TX FIFO and raise CE. Non-blocking.
 *  no_ack=true sends with the NO_ACK bit (fire and forget, needs EN_DYN_ACK). */
nrf24_status_t   nrf24_tx_push(const void *buf, uint8_t len, bool no_ack);

/** Advance the TX state machine. Call it often (every loop tick is fine). */
nrf24_tx_state_t nrf24_tx_service(void);

/** Convenience wrapper: push + spin until DONE/FAILED/timeout. */
nrf24_status_t   nrf24_tx_blocking(const void *buf, uint8_t len, bool no_ack,
                                   uint32_t timeout_us);

bool             nrf24_tx_fifo_full(void);
bool             nrf24_tx_fifo_empty(void);

/* ---- receive (PRX, or ACK payloads on the PTX side) ---- */

/** True if a payload is waiting; *pipe (may be NULL) gets the pipe index. */
bool     nrf24_rx_available(uint8_t *pipe);

/** Pop one payload. Returns bytes copied, 0 if none. */
uint8_t  nrf24_rx_read(void *buf, uint8_t maxlen);

/** PRX side: stage data to be returned inside the next ACK (max 32 B, 3-deep). */
nrf24_status_t nrf24_ack_payload_write(uint8_t pipe, const void *buf, uint8_t len);

/* ---- diagnostics ---- */

/** Received Power Detector latch: 1 = >-64 dBm seen while in RX. */
bool     nrf24_rpd(void);

/** Sweep channels [first, last] and report RPD hits per channel (dwell_us each).
 *  Use it at the pad to pick a channel away from Wi-Fi (Wi-Fi ch1/6/11 occupy
 *  roughly nRF channels 0-22 / 25-47 / 50-72). */
void     nrf24_scan(uint8_t first, uint8_t last, uint32_t dwell_us,
                    uint8_t *hits, uint8_t hits_len);

void     nrf24_get_stats(nrf24_stats_t *out);
uint8_t  nrf24_read_status(void);
uint8_t  nrf24_read_reg(uint8_t reg);
void     nrf24_write_reg(uint8_t reg, uint8_t val);
void     nrf24_flush_tx(void);
void     nrf24_flush_rx(void);

/** Dump CONFIG/EN_AA/RF_CH/RF_SETUP/FIFO_STATUS etc. into a caller buffer for
 *  printing over the debug UART. Returns bytes written. */
size_t   nrf24_dump(char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* NRF24_H */
