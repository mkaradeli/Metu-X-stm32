/**
 * @file    nrf24_regs.h
 * @brief   nRF24L01+ register map / command set (E01-ML01DP5 carries a genuine nRF24L01P).
 */
#ifndef NRF24_REGS_H
#define NRF24_REGS_H

/* ---- SPI commands ---- */
#define NRF24_CMD_R_REGISTER        0x00u   /* | addr(5b) */
#define NRF24_CMD_W_REGISTER        0x20u   /* | addr(5b) */
#define NRF24_CMD_R_RX_PAYLOAD      0x61u
#define NRF24_CMD_W_TX_PAYLOAD      0xA0u
#define NRF24_CMD_FLUSH_TX          0xE1u
#define NRF24_CMD_FLUSH_RX          0xE2u
#define NRF24_CMD_REUSE_TX_PL       0xE3u
#define NRF24_CMD_R_RX_PL_WID       0x60u
#define NRF24_CMD_W_ACK_PAYLOAD     0xA8u   /* | pipe(3b) */
#define NRF24_CMD_W_TX_PAYLOAD_NOACK 0xB0u
#define NRF24_CMD_ACTIVATE          0x50u   /* clones: followed by 0x73 */
#define NRF24_CMD_NOP               0xFFu

/* ---- Registers ---- */
#define NRF24_REG_CONFIG            0x00u
#define NRF24_REG_EN_AA             0x01u
#define NRF24_REG_EN_RXADDR         0x02u
#define NRF24_REG_SETUP_AW          0x03u
#define NRF24_REG_SETUP_RETR        0x04u
#define NRF24_REG_RF_CH             0x05u
#define NRF24_REG_RF_SETUP          0x06u
#define NRF24_REG_STATUS            0x07u
#define NRF24_REG_OBSERVE_TX        0x08u
#define NRF24_REG_RPD               0x09u
#define NRF24_REG_RX_ADDR_P0        0x0Au
#define NRF24_REG_RX_ADDR_P1        0x0Bu
#define NRF24_REG_RX_ADDR_P2        0x0Cu
#define NRF24_REG_RX_ADDR_P3        0x0Du
#define NRF24_REG_RX_ADDR_P4        0x0Eu
#define NRF24_REG_RX_ADDR_P5        0x0Fu
#define NRF24_REG_TX_ADDR           0x10u
#define NRF24_REG_RX_PW_P0          0x11u
#define NRF24_REG_RX_PW_P1          0x12u
#define NRF24_REG_FIFO_STATUS       0x17u
#define NRF24_REG_DYNPD             0x1Cu
#define NRF24_REG_FEATURE           0x1Du

/* ---- CONFIG ---- */
#define NRF24_CONFIG_MASK_RX_DR     (1u << 6)
#define NRF24_CONFIG_MASK_TX_DS     (1u << 5)
#define NRF24_CONFIG_MASK_MAX_RT    (1u << 4)
#define NRF24_CONFIG_EN_CRC         (1u << 3)
#define NRF24_CONFIG_CRCO           (1u << 2)   /* 1 = 2-byte CRC */
#define NRF24_CONFIG_PWR_UP         (1u << 1)
#define NRF24_CONFIG_PRIM_RX        (1u << 0)

/* ---- STATUS ---- */
#define NRF24_STATUS_RX_DR          (1u << 6)
#define NRF24_STATUS_TX_DS          (1u << 5)
#define NRF24_STATUS_MAX_RT         (1u << 4)
#define NRF24_STATUS_RX_P_NO_Msk    (0x07u << 1)
#define NRF24_STATUS_RX_P_NO_Pos    1u
#define NRF24_STATUS_TX_FULL        (1u << 0)

/* ---- FIFO_STATUS ---- */
#define NRF24_FIFO_RX_EMPTY         (1u << 0)
#define NRF24_FIFO_RX_FULL          (1u << 1)
#define NRF24_FIFO_TX_EMPTY         (1u << 4)
#define NRF24_FIFO_TX_FULL          (1u << 5)
#define NRF24_FIFO_TX_REUSE         (1u << 6)

/* ---- RF_SETUP ---- */
#define NRF24_RF_SETUP_CONT_WAVE    (1u << 7)
#define NRF24_RF_SETUP_RF_DR_LOW    (1u << 5)
#define NRF24_RF_SETUP_PLL_LOCK     (1u << 4)
#define NRF24_RF_SETUP_RF_DR_HIGH   (1u << 3)
#define NRF24_RF_SETUP_RF_PWR_Pos   1u
#define NRF24_RF_SETUP_RF_PWR_Msk   (0x03u << 1)

/* ---- FEATURE ---- */
#define NRF24_FEATURE_EN_DPL        (1u << 2)
#define NRF24_FEATURE_EN_ACK_PAY    (1u << 1)
#define NRF24_FEATURE_EN_DYN_ACK    (1u << 0)

/* ---- Datasheet timing (us) ---- */
#define NRF24_T_PWR_UP_US           1500u   /* power down -> standby-I (Tpd2stby, ext xtal) */
#define NRF24_T_STBY2A_US           130u    /* standby-I -> TX/RX active */
#define NRF24_T_HCE_US              10u     /* minimum CE high pulse */

#endif /* NRF24_REGS_H */
