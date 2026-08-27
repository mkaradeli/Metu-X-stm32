/**
 * @file    example_ground_rx.c
 * @brief   Ground side: receives telemetry frames, queues commands into the ACK
 *          payload, and forwards complete frames to the host (UART/USB -> Pi 5).
 */

#include <string.h>
#include "nrf24.h"
#include "nrf24_link.h"
#include "nrf24_port.h"

static uint8_t s_frame[NRF24_LINK_MAX_FRAME];

extern void host_write(const uint8_t *data, uint16_t len);   /* your UART/USB sink */

void ground_setup(void)
{
    nrf24_cfg_t cfg;

    nrf24_default_cfg(&cfg);
    cfg.channel     = 76u;
    cfg.data_rate   = NRF24_DR_1M;      /* MUST match the rocket */
    cfg.power       = NRF24_PWR_M6;
    cfg.ack_payload = true;

    /* The PRX listens on P0 with the address the PTX transmits to. */
    memcpy(cfg.rx_addr_p0, cfg.tx_addr, 5u);

    if (nrf24_init(&cfg) != NRF24_OK) {
        for (;;) { }
    }
    nrf24_link_init(false);
    nrf24_start_rx();
}

void ground_poll(void)
{
    uint16_t len = nrf24_link_poll(s_frame, sizeof(s_frame));

    if (len > 0u) {
        host_write(s_frame, len);       /* CRC already verified */
    }
}

/** Call when the operator sends a command; it goes out on the next ACK.
 *  Stage it early - it only leaves the board when the rocket transmits. */
void ground_queue_command(const uint8_t *cmd, uint8_t len)
{
    (void)nrf24_ack_payload_write(0u, cmd, len);
}

/** Optional: run once before a test to pick a quiet channel. */
void ground_channel_survey(uint8_t *hits, uint8_t n)
{
    nrf24_scan(0u, (uint8_t)(n - 1u), 20000u, hits, n);   /* 20 ms dwell per channel */
    nrf24_start_rx();
}
