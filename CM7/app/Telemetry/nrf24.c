/**
 * @file    nrf24.c
 * @brief   nRF24L01+ / E01-ML01DP5 driver core.
 */

#include <string.h>
#include <stdio.h>

#include "nrf24.h"
#include "nrf24_regs.h"
#include "nrf24_port.h"

#define SCRATCH_LEN   (NRF24_MAX_PAYLOAD + 1u)

static uint8_t        s_tx_buf[SCRATCH_LEN];
static uint8_t        s_rx_buf[SCRATCH_LEN];
static nrf24_cfg_t    s_cfg;
static nrf24_stats_t  s_stats;
static nrf24_tx_state_t s_tx_state = NRF24_TX_IDLE;
static uint32_t       s_tx_t0;
static uint32_t       s_tx_timeout_us = 20000u;   /* worst case ARC*(ARD+air) + margin */
static uint8_t        s_status;                   /* last STATUS byte seen             */
static bool           s_in_rx;

/* ------------------------------------------------------------- low level */

static uint8_t cmd_rw(uint8_t cmd, const uint8_t *tx, uint8_t *rx, uint8_t len)
{
    uint8_t i;

    if (len > NRF24_MAX_PAYLOAD) {
        len = NRF24_MAX_PAYLOAD;
    }
    s_tx_buf[0] = cmd;
    for (i = 0u; i < len; i++) {
        s_tx_buf[1u + i] = (tx != NULL) ? tx[i] : 0xFFu;
    }

    nrf24_port_csn(0);
    nrf24_port_spi_xfer(s_tx_buf, s_rx_buf, (size_t)len + 1u);
    nrf24_port_csn(1);

    if (rx != NULL) {
        for (i = 0u; i < len; i++) {
            rx[i] = s_rx_buf[1u + i];
        }
    }
    s_status = s_rx_buf[0];
    return s_status;
}

uint8_t nrf24_read_reg(uint8_t reg)
{
    uint8_t v = 0u;
    (void)cmd_rw(NRF24_CMD_R_REGISTER | (reg & 0x1Fu), NULL, &v, 1u);
    return v;
}

void nrf24_write_reg(uint8_t reg, uint8_t val)
{
    (void)cmd_rw(NRF24_CMD_W_REGISTER | (reg & 0x1Fu), &val, NULL, 1u);
}

static void write_reg_multi(uint8_t reg, const uint8_t *data, uint8_t len)
{
    (void)cmd_rw(NRF24_CMD_W_REGISTER | (reg & 0x1Fu), data, NULL, len);
}

uint8_t nrf24_read_status(void)
{
    (void)cmd_rw(NRF24_CMD_NOP, NULL, NULL, 0u);
    return s_status;
}

void nrf24_flush_tx(void) { (void)cmd_rw(NRF24_CMD_FLUSH_TX, NULL, NULL, 0u); }
void nrf24_flush_rx(void) { (void)cmd_rw(NRF24_CMD_FLUSH_RX, NULL, NULL, 0u); }

static void clear_irqs(uint8_t mask)
{
    nrf24_write_reg(NRF24_REG_STATUS, mask);
}

/* ------------------------------------------------------------ configuration */

void nrf24_default_cfg(nrf24_cfg_t *cfg)
{
    static const uint8_t addr_dl[5] = { 'M', 'E', 'T', 'U', 'X' };  /* rocket -> ground */
    static const uint8_t addr_ul[5] = { 'M', 'E', 'T', 'U', 'G' };  /* spare pipe       */

    if (cfg == NULL) {
        return;
    }
    memset(cfg, 0, sizeof(*cfg));
    cfg->channel     = 76u;            /* 2476 MHz: above Wi-Fi ch 11 */
    cfg->data_rate   = NRF24_DR_1M;
    cfg->power       = NRF24_PWR_0;
    cfg->addr_width  = 5u;
    memcpy(cfg->tx_addr,    addr_dl, 5u);
    memcpy(cfg->rx_addr_p0, addr_dl, 5u);   /* P0 must mirror TX_ADDR for auto-ack */
    memcpy(cfg->rx_addr_p1, addr_ul, 5u);
    cfg->payload_len = 0u;             /* dynamic length: needed for ACK payloads */
    cfg->auto_ack    = true;
    cfg->ack_payload = true;
    cfg->arc         = 2u;
    cfg->ard_us      = 500u;
    cfg->crc16       = true;
}

static uint8_t rf_setup_byte(nrf24_datarate_t dr, nrf24_power_t pwr)
{
    uint8_t v = (uint8_t)(((uint8_t)pwr << NRF24_RF_SETUP_RF_PWR_Pos) & NRF24_RF_SETUP_RF_PWR_Msk);

    switch (dr) {
    case NRF24_DR_250K: v |= NRF24_RF_SETUP_RF_DR_LOW;  break;   /* LOW=1, HIGH=0 */
    case NRF24_DR_2M:   v |= NRF24_RF_SETUP_RF_DR_HIGH; break;   /* LOW=0, HIGH=1 */
    case NRF24_DR_1M:
    default:                                            break;   /* both zero     */
    }
    return v;
}

static void write_feature(uint8_t feature, uint8_t dynpd)
{
    nrf24_write_reg(NRF24_REG_FEATURE, feature);
    if ((nrf24_read_reg(NRF24_REG_FEATURE) != feature) && (feature != 0u)) {
        /* Some clones need ACTIVATE 0x73 before FEATURE/DYNPD become writable. */
        uint8_t key = 0x73u;
        (void)cmd_rw(NRF24_CMD_ACTIVATE, &key, NULL, 1u);
        nrf24_write_reg(NRF24_REG_FEATURE, feature);
    }
    nrf24_write_reg(NRF24_REG_DYNPD, dynpd);
}

nrf24_status_t nrf24_init(const nrf24_cfg_t *cfg)
{
    uint8_t config, feature = 0u, dynpd = 0u, setup_retr;

    if ((cfg == NULL) || (cfg->channel > 125u) ||
        (cfg->addr_width < 3u) || (cfg->addr_width > 5u) ||
        (cfg->payload_len > 32u) || (cfg->arc > 15u) ||
        (cfg->ard_us < 250u) || (cfg->ard_us > 4000u)) {
        return NRF24_ERR_PARAM;
    }
    s_cfg = *cfg;
    memset(&s_stats, 0, sizeof(s_stats));

    nrf24_port_init();
    nrf24_port_ce(0);
    nrf24_port_delay_us(5000u);        /* let VDD/xtal settle after reset */

    /* Presence check: SETUP_AW is a benign R/W register. */
    nrf24_write_reg(NRF24_REG_SETUP_AW, (uint8_t)(s_cfg.addr_width - 2u));
    if (nrf24_read_reg(NRF24_REG_SETUP_AW) != (uint8_t)(s_cfg.addr_width - 2u)) {
        return NRF24_ERR_NO_RADIO;
    }

    /* CONFIG: power up, PTX, CRC, all IRQ sources unmasked (IRQ pin active). */
    config = NRF24_CONFIG_EN_CRC | NRF24_CONFIG_PWR_UP;
    if (s_cfg.crc16) {
        config |= NRF24_CONFIG_CRCO;
    }
    nrf24_write_reg(NRF24_REG_CONFIG, config);
    nrf24_port_delay_us(NRF24_T_PWR_UP_US + 500u);   /* Tpd2stby */

    nrf24_write_reg(NRF24_REG_EN_AA,     s_cfg.auto_ack ? 0x03u : 0x00u);  /* P0,P1 */
    nrf24_write_reg(NRF24_REG_EN_RXADDR, 0x03u);
    nrf24_write_reg(NRF24_REG_RF_CH,     s_cfg.channel);
    nrf24_write_reg(NRF24_REG_RF_SETUP,  rf_setup_byte(s_cfg.data_rate, s_cfg.power));

    setup_retr = (uint8_t)((((s_cfg.ard_us / 250u) - 1u) << 4) | (s_cfg.arc & 0x0Fu));
    nrf24_write_reg(NRF24_REG_SETUP_RETR, setup_retr);

    write_reg_multi(NRF24_REG_TX_ADDR,    s_cfg.tx_addr,    s_cfg.addr_width);
    write_reg_multi(NRF24_REG_RX_ADDR_P0, s_cfg.rx_addr_p0, s_cfg.addr_width);
    write_reg_multi(NRF24_REG_RX_ADDR_P1, s_cfg.rx_addr_p1, s_cfg.addr_width);

    if (s_cfg.payload_len == 0u) {
        feature |= NRF24_FEATURE_EN_DPL;
        dynpd    = 0x03u;
        nrf24_write_reg(NRF24_REG_RX_PW_P0, 0u);
        nrf24_write_reg(NRF24_REG_RX_PW_P1, 0u);
    } else {
        nrf24_write_reg(NRF24_REG_RX_PW_P0, s_cfg.payload_len);
        nrf24_write_reg(NRF24_REG_RX_PW_P1, s_cfg.payload_len);
    }
    if (s_cfg.ack_payload) {
        feature |= NRF24_FEATURE_EN_ACK_PAY | NRF24_FEATURE_EN_DPL;  /* ACK payload implies DPL */
        dynpd    = 0x03u;
    }
    feature |= NRF24_FEATURE_EN_DYN_ACK;    /* allows per-packet W_TX_PAYLOAD_NOACK */
    write_feature(feature, dynpd);

    nrf24_flush_tx();
    nrf24_flush_rx();
    clear_irqs(NRF24_STATUS_RX_DR | NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);

    /* Worst-case blocking time for one packet: (ARC+1) * (ARD + air time). */
    s_tx_timeout_us = (uint32_t)(s_cfg.arc + 1u) * (s_cfg.ard_us + 2000u) + 2000u;

    /* Final sanity: every critical register must read back. */
    if ((nrf24_read_reg(NRF24_REG_RF_CH) != s_cfg.channel) ||
        (nrf24_read_reg(NRF24_REG_SETUP_RETR) != setup_retr)) {
        return NRF24_ERR_NO_RADIO;
    }
    s_tx_state = NRF24_TX_IDLE;
    s_in_rx    = false;
    return NRF24_OK;
}

nrf24_status_t nrf24_set_channel(uint8_t ch)
{
    if (ch > 125u) {
        return NRF24_ERR_PARAM;
    }
    s_cfg.channel = ch;
    nrf24_write_reg(NRF24_REG_RF_CH, ch);
    return NRF24_OK;
}

nrf24_status_t nrf24_set_datarate(nrf24_datarate_t dr)
{
    s_cfg.data_rate = dr;
    nrf24_write_reg(NRF24_REG_RF_SETUP, rf_setup_byte(dr, s_cfg.power));
    return NRF24_OK;
}

nrf24_status_t nrf24_set_power(nrf24_power_t pwr)
{
    s_cfg.power = pwr;
    nrf24_write_reg(NRF24_REG_RF_SETUP, rf_setup_byte(s_cfg.data_rate, pwr));
    return NRF24_OK;
}

/* ------------------------------------------------------------ mode control */

void nrf24_standby(void)
{
    nrf24_port_ce(0);
    s_in_rx = false;
}

void nrf24_power_down(void)
{
    nrf24_port_ce(0);
    nrf24_write_reg(NRF24_REG_CONFIG,
                    (uint8_t)(nrf24_read_reg(NRF24_REG_CONFIG) & ~NRF24_CONFIG_PWR_UP));
    s_in_rx = false;
}

void nrf24_start_rx(void)
{
    uint8_t config = nrf24_read_reg(NRF24_REG_CONFIG);

    nrf24_port_ce(0);
    /* RX_ADDR_P0 is shared with the auto-ack path; restore the listening address. */
    write_reg_multi(NRF24_REG_RX_ADDR_P0, s_cfg.rx_addr_p0, s_cfg.addr_width);
    nrf24_write_reg(NRF24_REG_CONFIG, (uint8_t)(config | NRF24_CONFIG_PWR_UP | NRF24_CONFIG_PRIM_RX));
    clear_irqs(NRF24_STATUS_RX_DR | NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);
    nrf24_flush_rx();
    nrf24_port_ce(1);
    nrf24_port_delay_us(NRF24_T_STBY2A_US);
    s_in_rx    = true;
    s_tx_state = NRF24_TX_IDLE;
}

void nrf24_start_tx(void)
{
    uint8_t config = nrf24_read_reg(NRF24_REG_CONFIG);

    nrf24_port_ce(0);
    nrf24_write_reg(NRF24_REG_CONFIG,
                    (uint8_t)((config | NRF24_CONFIG_PWR_UP) & ~NRF24_CONFIG_PRIM_RX));
    nrf24_port_delay_us(NRF24_T_STBY2A_US);
    s_in_rx    = false;
    s_tx_state = NRF24_TX_IDLE;
}

/* ---------------------------------------------------------------- transmit */

bool nrf24_tx_fifo_full(void)
{
    return (nrf24_read_reg(NRF24_REG_FIFO_STATUS) & NRF24_FIFO_TX_FULL) != 0u;
}

bool nrf24_tx_fifo_empty(void)
{
    return (nrf24_read_reg(NRF24_REG_FIFO_STATUS) & NRF24_FIFO_TX_EMPTY) != 0u;
}

nrf24_status_t nrf24_tx_push(const void *buf, uint8_t len, bool no_ack)
{
    uint8_t padded[NRF24_MAX_PAYLOAD];
    uint8_t n;

    if ((buf == NULL) || (len == 0u) || (len > NRF24_MAX_PAYLOAD)) {
        return NRF24_ERR_PARAM;
    }
    if (s_in_rx) {
        nrf24_start_tx();
    }
    if (nrf24_tx_fifo_full()) {
        return NRF24_ERR_FIFO_FULL;
    }

    if (s_cfg.payload_len == 0u) {
        n = len;                                   /* dynamic payload length */
        memcpy(padded, buf, len);
    } else {
        n = s_cfg.payload_len;                     /* fixed: zero-pad the tail */
        memset(padded, 0, n);
        memcpy(padded, buf, (len < n) ? len : n);
    }

    (void)cmd_rw(no_ack ? NRF24_CMD_W_TX_PAYLOAD_NOACK : NRF24_CMD_W_TX_PAYLOAD,
                 padded, NULL, n);

    nrf24_port_ce(1);                              /* hold CE while the FIFO drains */
    if (s_tx_state != NRF24_TX_BUSY) {
        s_tx_t0 = nrf24_port_micros();
    }
    s_tx_state = NRF24_TX_BUSY;
    return NRF24_OK;
}

nrf24_tx_state_t nrf24_tx_service(void)
{
    uint8_t status, obs;

    if (s_tx_state != NRF24_TX_BUSY) {
        return s_tx_state;
    }

    status = nrf24_read_status();

    if ((status & NRF24_STATUS_MAX_RT) != 0u) {
        obs = nrf24_read_reg(NRF24_REG_OBSERVE_TX);
        s_stats.retransmits += (uint32_t)(obs & 0x0Fu);
        s_stats.lost_packets = (uint32_t)(obs >> 4);
        s_stats.tx_dropped++;
        clear_irqs(NRF24_STATUS_MAX_RT);
        nrf24_flush_tx();          /* the stuck payload would block the FIFO forever */
        nrf24_port_ce(0);
        s_tx_state = NRF24_TX_FAILED;
        return s_tx_state;
    }

    if ((status & NRF24_STATUS_TX_DS) != 0u) {
        obs = nrf24_read_reg(NRF24_REG_OBSERVE_TX);
        s_stats.retransmits += (uint32_t)(obs & 0x0Fu);
        s_stats.tx_ok++;
        clear_irqs(NRF24_STATUS_TX_DS);
        if (nrf24_tx_fifo_empty()) {
            nrf24_port_ce(0);      /* never sit in TX longer than needed (4 ms limit) */
            s_tx_state = NRF24_TX_DONE;
        } else {
            s_tx_t0 = nrf24_port_micros();
        }
        return s_tx_state;
    }

    if ((nrf24_port_micros() - s_tx_t0) > s_tx_timeout_us) {
        s_stats.tx_timeout++;
        nrf24_flush_tx();
        nrf24_port_ce(0);
        clear_irqs(NRF24_STATUS_RX_DR | NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);
        s_tx_state = NRF24_TX_FAILED;
    }
    return s_tx_state;
}

nrf24_status_t nrf24_tx_blocking(const void *buf, uint8_t len, bool no_ack,
                                 uint32_t timeout_us)
{
    nrf24_status_t rc = nrf24_tx_push(buf, len, no_ack);
    uint32_t t0;

    if (rc != NRF24_OK) {
        return rc;
    }
    t0 = nrf24_port_micros();
    for (;;) {
        nrf24_tx_state_t st = nrf24_tx_service();
        if (st == NRF24_TX_DONE) {
            s_tx_state = NRF24_TX_IDLE;
            return NRF24_OK;
        }
        if (st == NRF24_TX_FAILED) {
            s_tx_state = NRF24_TX_IDLE;
            return NRF24_ERR_MAX_RT;
        }
        if ((nrf24_port_micros() - t0) > timeout_us) {
            return NRF24_ERR_TIMEOUT;
        }
    }
}

/* ----------------------------------------------------------------- receive */

bool nrf24_rx_available(uint8_t *pipe)
{
    uint8_t status = nrf24_read_status();
    uint8_t p = (uint8_t)((status & NRF24_STATUS_RX_P_NO_Msk) >> NRF24_STATUS_RX_P_NO_Pos);

    if (p > 5u) {
        /* Fall back to the FIFO flag: STATUS is only refreshed on RX_DR. */
        if ((nrf24_read_reg(NRF24_REG_FIFO_STATUS) & NRF24_FIFO_RX_EMPTY) != 0u) {
            return false;
        }
        p = 0u;
    }
    if (pipe != NULL) {
        *pipe = p;
    }
    return true;
}

uint8_t nrf24_rx_read(void *buf, uint8_t maxlen)
{
    uint8_t len;

    if ((buf == NULL) || (maxlen == 0u)) {
        return 0u;
    }
    if ((nrf24_read_reg(NRF24_REG_FIFO_STATUS) & NRF24_FIFO_RX_EMPTY) != 0u) {
        return 0u;
    }

    if ((s_cfg.payload_len == 0u) || s_cfg.ack_payload) {
        (void)cmd_rw(NRF24_CMD_R_RX_PL_WID, NULL, &len, 1u);
        if ((len == 0u) || (len > NRF24_MAX_PAYLOAD)) {
            nrf24_flush_rx();                 /* datasheet: >32 means corrupt FIFO */
            clear_irqs(NRF24_STATUS_RX_DR);
            return 0u;
        }
    } else {
        len = s_cfg.payload_len;
    }

    (void)cmd_rw(NRF24_CMD_R_RX_PAYLOAD, NULL, (uint8_t *)buf,
                 (len < maxlen) ? len : maxlen);
    clear_irqs(NRF24_STATUS_RX_DR);
    s_stats.rx_ok++;
    return (len < maxlen) ? len : maxlen;
}

nrf24_status_t nrf24_ack_payload_write(uint8_t pipe, const void *buf, uint8_t len)
{
    if ((buf == NULL) || (pipe > 5u) || (len == 0u) || (len > NRF24_MAX_PAYLOAD)) {
        return NRF24_ERR_PARAM;
    }
    if (nrf24_tx_fifo_full()) {
        return NRF24_ERR_FIFO_FULL;   /* ACK payloads live in the TX FIFO */
    }
    (void)cmd_rw((uint8_t)(NRF24_CMD_W_ACK_PAYLOAD | pipe), (const uint8_t *)buf, NULL, len);
    return NRF24_OK;
}

/* ------------------------------------------------------------- diagnostics */

bool nrf24_rpd(void)
{
    return (nrf24_read_reg(NRF24_REG_RPD) & 0x01u) != 0u;
}

void nrf24_scan(uint8_t first, uint8_t last, uint32_t dwell_us,
                uint8_t *hits, uint8_t hits_len)
{
    uint8_t ch;
    uint8_t idx = 0u;
    uint8_t saved = s_cfg.channel;

    if ((hits == NULL) || (last > 125u) || (first > last)) {
        return;
    }
    for (ch = first; (ch <= last) && (idx < hits_len); ch++, idx++) {
        uint32_t t0;
        uint8_t  n = 0u;

        nrf24_standby();
        (void)nrf24_set_channel(ch);
        nrf24_start_rx();
        t0 = nrf24_port_micros();
        while ((nrf24_port_micros() - t0) < dwell_us) {
            if (nrf24_rpd()) {
                if (n < 255u) {
                    n++;
                }
                nrf24_standby();      /* RPD latches; re-enter RX to re-arm it */
                nrf24_port_ce(1);
                nrf24_port_delay_us(NRF24_T_STBY2A_US);
            }
        }
        hits[idx] = n;
    }
    nrf24_standby();
    (void)nrf24_set_channel(saved);
}

void nrf24_get_stats(nrf24_stats_t *out)
{
    if (out != NULL) {
        *out = s_stats;
    }
}

size_t nrf24_dump(char *buf, size_t len)
{
    int n;

    if ((buf == NULL) || (len == 0u)) {
        return 0u;
    }
    n = snprintf(buf, len,
                 "CFG=%02X EN_AA=%02X EN_RX=%02X AW=%02X RETR=%02X CH=%u RF=%02X "
                 "STAT=%02X OBS=%02X FIFO=%02X DYNPD=%02X FEAT=%02X | ok=%lu drop=%lu rx=%lu rt=%lu\r\n",
                 nrf24_read_reg(NRF24_REG_CONFIG),
                 nrf24_read_reg(NRF24_REG_EN_AA),
                 nrf24_read_reg(NRF24_REG_EN_RXADDR),
                 nrf24_read_reg(NRF24_REG_SETUP_AW),
                 nrf24_read_reg(NRF24_REG_SETUP_RETR),
                 nrf24_read_reg(NRF24_REG_RF_CH),
                 nrf24_read_reg(NRF24_REG_RF_SETUP),
                 nrf24_read_status(),
                 nrf24_read_reg(NRF24_REG_OBSERVE_TX),
                 nrf24_read_reg(NRF24_REG_FIFO_STATUS),
                 nrf24_read_reg(NRF24_REG_DYNPD),
                 nrf24_read_reg(NRF24_REG_FEATURE),
                 (unsigned long)s_stats.tx_ok,
                 (unsigned long)s_stats.tx_dropped,
                 (unsigned long)s_stats.rx_ok,
                 (unsigned long)s_stats.retransmits);
    return (n < 0) ? 0u : (size_t)n;
}
