/**
 * @file    example_rocket_tx.c
 * @brief   Airborne side: streams telemetry frames down, receives commands back
 *          inside the ACK payload. Nothing here blocks longer than one SPI burst.
 */

#include <string.h>
#include "nrf24.h"
#include "nrf24_link.h"
#include "nrf24_port.h"

#define TELEM_FRAME_LEN   317u
#define TELEM_PERIOD_US   20000u        /* 50 Hz */

static uint8_t s_frame[TELEM_FRAME_LEN];

static void handle_uplink_command(const uint8_t *cmd, uint8_t len);

void radio_setup(void)
{
    nrf24_cfg_t cfg;

    nrf24_default_cfg(&cfg);
    cfg.channel   = 76u;                /* 2476 MHz */
    cfg.data_rate = NRF24_DR_1M;        /* 1 Mbps: 45 dB of margin at 30 m       */
    cfg.power     = NRF24_PWR_M6;       /* ~+14 dBm at the SMA; see README budget */
    cfg.arc       = 2u;                 /* cap the worst-case stall at ~1.4 ms    */
    cfg.ard_us    = 500u;
    cfg.ack_payload = true;             /* ground -> rocket commands ride the ACK */

    if (nrf24_init(&cfg) != NRF24_OK) {
        /* MISO stuck: check CSN/SCK/MOSI wiring and that the radio LDO is up. */
        for (;;) { }
    }
    nrf24_link_init(false);             /* auto-ack ON so ACK payloads work */
    nrf24_start_tx();
}

void radio_task_1khz(void)
{
    static uint32_t t_last;
    uint32_t now = nrf24_port_micros();

    /* 1. Always pump the state machine first. */
    (void)nrf24_link_service();

    /* 2. Drain any command that came back inside an ACK. */
    while (nrf24_rx_available(NULL)) {
        uint8_t cmd[NRF24_MAX_PAYLOAD];
        uint8_t n = nrf24_rx_read(cmd, sizeof(cmd));
        if (n > 0u) {
            handle_uplink_command(cmd, n);
        }
    }

    /* 3. Start a new frame only when the previous one has cleared. */
    if (((now - t_last) >= TELEM_PERIOD_US) && nrf24_link_tx_idle()) {
        t_last = now;
        /* telemetry_pack(s_frame, sizeof(s_frame)); */
        (void)nrf24_link_send(s_frame, TELEM_FRAME_LEN);
    }
}

static void handle_uplink_command(const uint8_t *cmd, uint8_t len)
{
    (void)cmd;
    (void)len;
    /* Route into MissionControl: ARMED / DROP / HOVER / SAFE_DISCHARGE.
     * Keep the confirmation handshake - an ACK payload proves the radio link
     * delivered the byte, not that the command was accepted. */
}
