/**
 * @file    nrf24_link.c
 */

#include <string.h>

#include "nrf24_link.h"
#include "nrf24_regs.h"
#include "nrf24_port.h"

#define HDR_LAST_BIT   0x80u

typedef struct {
    uint8_t  buf[NRF24_LINK_MAX_FRAME + 2u];   /* frame + CRC */
    uint16_t len;
    uint16_t sent;          /* bytes already handed to the radio */
    uint8_t  seq;
    uint8_t  frag;
    bool     active;
} tx_ctx_t;

typedef struct {
    uint8_t  buf[NRF24_LINK_MAX_FRAME + 2u];
    uint16_t len;
    uint8_t  seq;
    uint8_t  next_frag;
    bool     active;
} rx_ctx_t;

static tx_ctx_t           s_tx;
static rx_ctx_t           s_rx;
static nrf24_link_stats_t s_stats;
static bool               s_no_ack;
static uint8_t            s_seq_counter;

uint16_t nrf24_crc16_ccitt_false(const uint8_t *data, uint16_t len, uint16_t seed)
{
    uint16_t crc = seed;
    uint16_t i;
    uint8_t  b;

    for (i = 0u; i < len; i++) {
        crc ^= (uint16_t)((uint16_t)data[i] << 8);
        for (b = 0u; b < 8u; b++) {
            crc = (crc & 0x8000u) ? (uint16_t)((crc << 1) ^ 0x1021u) : (uint16_t)(crc << 1);
        }
    }
    return crc;
}

void nrf24_link_init(bool no_ack_downlink)
{
    memset(&s_tx, 0, sizeof(s_tx));
    memset(&s_rx, 0, sizeof(s_rx));
    memset(&s_stats, 0, sizeof(s_stats));
    s_no_ack      = no_ack_downlink;
    s_seq_counter = 0u;
}

bool nrf24_link_tx_idle(void)
{
    return !s_tx.active;
}

bool nrf24_link_send(const void *frame, uint16_t len)
{
    uint16_t crc;

    if ((frame == NULL) || (len == 0u) || (len > NRF24_LINK_MAX_FRAME) || s_tx.active) {
        return false;
    }

    /* CRC must cover exactly what got copied, not re-read `frame` a second
     * time: local_sensor_data is written from pressure_adc_complete() (an
     * ADC ISR), which can preempt this function between the copy and the
     * CRC and change bytes out from under a second read of `frame`. Hashing
     * s_tx.buf instead means the CRC always matches whatever this call
     * actually captured, torn or not - a torn snapshot across an ISR
     * boundary is an acceptable telemetry trade-off, a self-inconsistent
     * CRC on every send is not. */
    memcpy(s_tx.buf, frame, len);
    crc = nrf24_crc16_ccitt_false(s_tx.buf, len, 0xFFFFu);
    s_tx.buf[len]      = (uint8_t)(crc >> 8);
    s_tx.buf[len + 1u] = (uint8_t)(crc & 0xFFu);

    s_tx.len    = (uint16_t)(len + 2u);
    s_tx.sent   = 0u;
    s_tx.frag   = 0u;
    s_tx.seq    = s_seq_counter++;
    s_tx.active = true;
    return true;
}

bool nrf24_link_service(void)
{
    uint8_t pkt[NRF24_MAX_PAYLOAD];

    if (!s_tx.active) {
        (void)nrf24_tx_service();
        return false;
    }

    /* Keep the 3-deep FIFO topped up; stop as soon as it is full. */
    while ((s_tx.sent < s_tx.len) && !nrf24_tx_fifo_full()) {
        uint16_t remaining = (uint16_t)(s_tx.len - s_tx.sent);
        uint8_t  chunk     = (remaining > NRF24_LINK_FRAG_DATA)
                             ? (uint8_t)NRF24_LINK_FRAG_DATA : (uint8_t)remaining;
        bool     last      = (chunk == remaining);

        pkt[0] = s_tx.seq;
        pkt[1] = (uint8_t)((s_tx.frag & 0x7Fu) | (last ? HDR_LAST_BIT : 0u));
        memset(&pkt[2], 0, NRF24_LINK_FRAG_DATA);
        memcpy(&pkt[2], &s_tx.buf[s_tx.sent], chunk);

        /* With dynamic payload length the tail fragment goes out short, so no
         * padding reaches the air. In fixed-32 mode nrf24_tx_push() pads and the
         * receiver strips it below. */
        if (nrf24_tx_push(pkt, (uint8_t)(NRF24_LINK_HDR_LEN + chunk), s_no_ack) != NRF24_OK) {
            break;
        }
        s_tx.sent = (uint16_t)(s_tx.sent + chunk);
        s_tx.frag++;
        s_stats.frags_sent++;
    }

    {
        nrf24_tx_state_t st = nrf24_tx_service();

        if (st == NRF24_TX_FAILED) {
            s_stats.frames_dropped++;
            s_tx.active = false;              /* stale telemetry is worthless: move on */
        } else if ((st == NRF24_TX_DONE) && (s_tx.sent >= s_tx.len)) {
            s_stats.frames_sent++;
            s_tx.active = false;
        } else {
            /* still draining */
        }
    }
    return s_tx.active;
}

uint16_t nrf24_link_poll(void *out, uint16_t maxlen)
{
    uint8_t  pkt[NRF24_MAX_PAYLOAD];
    uint8_t  n;
    uint8_t  seq, frag;
    bool     last;
    uint16_t chunk;

    if ((out == NULL) || !nrf24_rx_available(NULL)) {
        return 0u;
    }

    n = nrf24_rx_read(pkt, (uint8_t)sizeof(pkt));
    if (n < (NRF24_LINK_HDR_LEN + 1u)) {
        return 0u;
    }

    seq  = pkt[0];
    frag = (uint8_t)(pkt[1] & 0x7Fu);
    last = (pkt[1] & HDR_LAST_BIT) != 0u;
    chunk = (uint16_t)(n - NRF24_LINK_HDR_LEN);

    if (frag == 0u) {
        s_rx.active    = true;
        s_rx.seq       = seq;
        s_rx.len       = 0u;
        s_rx.next_frag = 0u;
    } else if (!s_rx.active || (seq != s_rx.seq) || (frag != s_rx.next_frag)) {
        s_stats.reassembly_drops++;
        s_rx.active = false;
        return 0u;
    }

    if ((uint32_t)s_rx.len + chunk > sizeof(s_rx.buf)) {
        s_stats.reassembly_drops++;
        s_rx.active = false;
        return 0u;
    }
    memcpy(&s_rx.buf[s_rx.len], &pkt[NRF24_LINK_HDR_LEN], chunk);
    s_rx.len = (uint16_t)(s_rx.len + chunk);
    s_rx.next_frag++;

    if (!last) {
        return 0u;
    }
    s_rx.active = false;

    /* Fixed 32-byte payloads mean the tail fragment is zero-padded; the CRC tells
     * us where the real frame ended, so walk back over the padding. */
    {
        uint16_t len = s_rx.len;

        while (len >= 3u) {
            uint16_t crc_rx = (uint16_t)(((uint16_t)s_rx.buf[len - 2u] << 8) | s_rx.buf[len - 1u]);
            uint16_t body   = (uint16_t)(len - 2u);

            if (nrf24_crc16_ccitt_false(s_rx.buf, body, 0xFFFFu) == crc_rx) {
                if (body > maxlen) {
                    body = maxlen;
                }
                memcpy(out, s_rx.buf, body);
                s_stats.frames_rx++;
                return body;
            }
            if (s_rx.buf[len - 1u] != 0u) {
                break;                       /* not padding: a real CRC error */
            }
            len--;                           /* strip one pad byte and retry  */
        }
    }

    s_stats.crc_errors++;
    return 0u;
}

void nrf24_link_get_stats(nrf24_link_stats_t *out)
{
    if (out != NULL) {
        *out = s_stats;
    }
}
