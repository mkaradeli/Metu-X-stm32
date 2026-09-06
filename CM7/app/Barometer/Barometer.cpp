/*
 * Barometer.cpp
 *
 *  Created on: Sep 6, 2026
 *      Author: karadeli
 */
#include "Barometer.hpp"

#include <cmath>

/* Register map / bitfields per Bosch's BMP5-Sensor-API (bmp5_defs.h). */
namespace {
    constexpr uint8_t REG_CHIP_ID        = 0x01;
    constexpr uint8_t REG_TEMP_DATA_XLSB = 0x1D; /* burst: temp(3) + press(3) */
    constexpr uint8_t REG_OSR_CONFIG     = 0x36;
    constexpr uint8_t REG_ODR_CONFIG     = 0x37;
    constexpr uint8_t REG_CMD            = 0x7E;

    constexpr uint8_t CHIP_ID_PRIM    = 0x50;
    constexpr uint8_t CHIP_ID_SEC     = 0x51;
    constexpr uint8_t CMD_SOFT_RESET  = 0xB6;

    /* OSR_CONFIG (0x36): osr_t[2:0], osr_p[5:3], press_en[6] */
    constexpr uint8_t OSR_TEMP_1X   = 0x00;
    constexpr uint8_t OSR_PRESS_8X  = 0x03;
    constexpr uint8_t PRESS_EN_BIT  = 0x40;

    /* ODR_CONFIG (0x37): powermode[1:0], odr[6:2], deep_dis[7] */
    constexpr uint8_t ODR_50_HZ            = 0x0F;
    constexpr uint8_t POWERMODE_CONTINUOUS = 0x03;
    constexpr uint8_t DEEP_DISABLED_BIT    = 0x80;

    constexpr uint32_t I2C_TIMEOUT_MS = 10;
}

uint8_t Barometer::rxBuffer_[6] __attribute__((section(".sram4")));

Barometer::Barometer(I2C_HandleTypeDef* i2cHandle, uint8_t i2cAddr7bit)
    : i2c_(i2cHandle), i2cAddr8_(static_cast<uint16_t>(i2cAddr7bit) << 1) {}

bool Barometer::writeReg(uint8_t reg, uint8_t value) {
    return HAL_I2C_Mem_Write(i2c_, i2cAddr8_, reg, I2C_MEMADD_SIZE_8BIT,
                              &value, 1, I2C_TIMEOUT_MS) == HAL_OK;
}

bool Barometer::readReg(uint8_t reg, uint8_t* value) {
    return HAL_I2C_Mem_Read(i2c_, i2cAddr8_, reg, I2C_MEMADD_SIZE_8BIT,
                             value, 1, I2C_TIMEOUT_MS) == HAL_OK;
}

bool Barometer::init() {
    if (!writeReg(REG_CMD, CMD_SOFT_RESET))
        return false;
    HAL_Delay(3); /* datasheet: soft reset takes ~2 ms */

    uint8_t chipId = 0;
    if (!readReg(REG_CHIP_ID, &chipId))
        return false;
    if (chipId != CHIP_ID_PRIM && chipId != CHIP_ID_SEC)
        return false;

    const uint8_t osr = static_cast<uint8_t>(OSR_TEMP_1X
                       | (OSR_PRESS_8X << 3)
                       | PRESS_EN_BIT);
    if (!writeReg(REG_OSR_CONFIG, osr))
        return false;

    const uint8_t odr = static_cast<uint8_t>(POWERMODE_CONTINUOUS
                       | (ODR_50_HZ << 2)
                       | DEEP_DISABLED_BIT);
    if (!writeReg(REG_ODR_CONFIG, odr))
        return false;

    readInProgress_ = false;
    newReading_ = false;
    return true;
}

bool Barometer::startRead() {
    if (readInProgress_)
        return false;

    readInProgress_ = true;
    const HAL_StatusTypeDef st = HAL_I2C_Mem_Read_DMA(
        i2c_, i2cAddr8_, REG_TEMP_DATA_XLSB, I2C_MEMADD_SIZE_8BIT,
        rxBuffer_, sizeof(rxBuffer_));

    if (st != HAL_OK) {
        readInProgress_ = false;
        return false;
    }
    return true;
}

void Barometer::onReadComplete() {
    readInProgress_ = false;

    int32_t rawT = static_cast<int32_t>((static_cast<uint32_t>(rxBuffer_[2]) << 16)
                                       | (static_cast<uint32_t>(rxBuffer_[1]) << 8)
                                       |  static_cast<uint32_t>(rxBuffer_[0]));
    if (rawT & 0x00800000)          /* sign-extend 24-bit two's complement */
        rawT |= static_cast<int32_t>(0xFF000000);

    const uint32_t rawP = (static_cast<uint32_t>(rxBuffer_[5]) << 16)
                         | (static_cast<uint32_t>(rxBuffer_[4]) << 8)
                         |  static_cast<uint32_t>(rxBuffer_[3]);

    temperatureC_ = static_cast<float>(rawT) / 65536.0f;
    pressurePa_   = static_cast<float>(rawP) / 64.0f;
    newReading_   = true;
}

void Barometer::onReadError() {
    readInProgress_ = false;
}

bool Barometer::hasNewReading() {
    const bool r = newReading_;
    newReading_ = false;
    return r;
}

float Barometer::getAltitudeM() const {
    if (referencePa_ <= 0.0f || pressurePa_ <= 0.0f)
        return 0.0f;
    return 44330.0f * (1.0f - std::pow(pressurePa_ / referencePa_, 1.0f / 5.255f));
}
