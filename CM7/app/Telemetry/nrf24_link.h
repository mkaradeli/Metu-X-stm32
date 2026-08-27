/**
 * @file    nrf24_link.h
 * @brief   Fragmentation / reassembly layer so you can push frames larger than
 *          the radio's 32-byte payload (e.g. a 317-byte telemetry frame) across
 *          the link without blocking the control loop.
 *
 * Wire format of every radio payload (fixed 32 bytes):
 *
 *   byte 0      seq        frame sequence number, wraps at 256
 *   byte 1      bit7 LAST  1 on the final fragment of the frame
 *               bit6:0     fragment index, 0..127
 *   byte 2..31  data       up to 30 payload bytes (tail fragment zero-padded)
 *
 * The last fragment carries a CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF,
 * no reflection, no final xor) computed over the whole user frame, appended
 * big-endian - the same CRC your SD logger and YMODEM path already use, so the
 * ground station can share one implementation.
 *
 * Overhead: a 317-byte frame + 2 CRC bytes = 319 -> ceil(319/30) = 11 fragments.
 * With dynamic payload length that is 10*32 + 21 = 341 radio bytes (+7.6 %);
 * with fixed 32-byte payloads it is 11*32 = 352 (+11.0 %).
 */
#ifndef NRF24_LINK_H
#define NRF24_LINK_H

#include <stdint.h>
#include <stdbool.h>
#include "nrf24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NRF24_LINK_HDR_LEN     2u
#define NRF24_LINK_FRAG_DATA   (NRF24_MAX_PAYLOAD - NRF24_LINK_HDR_LEN)   /* 30 */
#define NRF24_LINK_MAX_FRAME   512u

typedef struct {
    uint32_t frames_sent;
    uint32_t frames_dropped;   /* a fragment was lost on the way out           */
    uint32_t frags_sent;
    uint32_t frames_rx;
    uint32_t crc_errors;
    uint32_t reassembly_drops; /* out-of-order / gapped fragment stream        */
} nrf24_link_stats_t;

/** Reset the link state machines. Call after nrf24_init(). */
void nrf24_link_init(bool no_ack_downlink);

/** Queue one frame for transmission. Returns false if the previous frame has
 *  not finished yet (one frame in flight; check nrf24_link_tx_idle()). */
bool nrf24_link_send(const void *frame, uint16_t len);

bool nrf24_link_tx_idle(void);

/** Pump the TX fragment machine + radio state machine. Call every loop tick.
 *  Returns true while a frame is still in flight. Never blocks. */
bool nrf24_link_service(void);

/** Feed received radio payloads into the reassembler. Returns the length of a
 *  completed, CRC-verified frame (copied into out) or 0. Call it in a loop. */
uint16_t nrf24_link_poll(void *out, uint16_t maxlen);

void nrf24_link_get_stats(nrf24_link_stats_t *out);

/** CRC-16/CCITT-FALSE, exposed so the same routine can be reused elsewhere. */
uint16_t nrf24_crc16_ccitt_false(const uint8_t *data, uint16_t len, uint16_t seed);

#ifdef __cplusplus
}
#endif

#endif /* NRF24_LINK_H */
