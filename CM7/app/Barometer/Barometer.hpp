/*
 * Barometer.hpp
 *
 *  Created on: Sep 6, 2026
 *      Author: karadeli
 */

#ifndef BAROMETER_BAROMETER_HPP_
#define BAROMETER_BAROMETER_HPP_

#include <stdint.h>
#include "i2c.h"

/*
 * Driver for the Bosch BMP581 barometric pressure sensor on I2C4
 * (BARO_I2C4_SDA/SCL, PF15/PD12). BMP581 outputs already-compensated
 * pressure/temperature, so unlike older Bosch sensors (BMP280/388) no
 * calibration coefficients need to be read or applied.
 *
 * Read path is non-blocking: startRead() kicks a DMA burst read of the
 * temperature+pressure registers, onReadComplete() parses it. I2C4 sits in
 * the H755's D3 power domain, so its DMA requests can only be serviced by
 * BDMA/DMAMUX2 (never DMA1/DMA2) -- and BDMA can only reach D3 SRAM. That is
 * why rxBuffer_ is pinned to the .sram4 linker section: placing it anywhere
 * else makes the DMA transfer silently fail.
 *
 * init() is blocking (polled I2C) and is only meant to run once at startup.
 */
class Barometer {
public:
    static constexpr uint8_t kI2cAddrPrimary   = 0x46; /* SDO tied low  */
    static constexpr uint8_t kI2cAddrSecondary = 0x47; /* SDO tied high */

    explicit Barometer(I2C_HandleTypeDef* i2cHandle, uint8_t i2cAddr7bit = kI2cAddrPrimary);

    /* Soft-resets the sensor, checks CHIP_ID, and configures oversampling /
     * ODR / power mode. Blocking. Call once at startup before startRead(). */
    bool init();

    /* Kicks off a non-blocking DMA read of temperature+pressure. Returns
     * false if a read is already in flight or the DMA request could not be
     * queued. Call periodically once init() has succeeded. */
    bool startRead();

    /* Call from HAL_I2C_MemRxCpltCallback when hi2c matches this instance's
     * handle. Parses the finished transfer into pressure/temperature. Does
     * not re-arm the next read -- that is the caller's responsibility, same
     * as the Lidar driver's re-arm-in-the-ISR-callback convention. */
    void onReadComplete();

    /* Call from HAL_I2C_ErrorCallback when hi2c matches this instance's
     * handle, so a bus error can't leave a read permanently "in flight". */
    void onReadError();

    bool isReadInProgress() const { return readInProgress_; }

    /* Clear-on-read new-data flag, mirroring Lidar::hasNewReading(). */
    bool hasNewReading();

    float getPressurePa()   const { return pressurePa_; }
    float getTemperatureC() const { return temperatureC_; }

    /* Ground-level reference pressure used by getAltitudeM(). Capture this
     * once on the pad, e.g. by averaging getPressurePa() for ~1-2 s, the
     * same way AltitudeEstimator calibrates its lidar height baseline. */
    void  setReferencePressure(float pa) { referencePa_ = pa; }
    float getReferencePressure() const   { return referencePa_; }

    /* Relative altitude above the reference pressure, international
     * barometric formula. Positive = up. Not filtered -- feed this into
     * AltitudeEstimator (or your own low-pass) rather than using it raw. */
    float getAltitudeM() const;

private:
    bool writeReg(uint8_t reg, uint8_t value);
    bool readReg(uint8_t reg, uint8_t* value);

    I2C_HandleTypeDef* i2c_;
    uint16_t i2cAddr8_; /* HAL wants the address pre-shifted (7-bit << 1) */

    float pressurePa_   = 0.0f;
    float temperatureC_ = 0.0f;
    float referencePa_  = 101325.0f;

    volatile bool readInProgress_ = false;
    bool newReading_ = false;

    /* Shared across instances by construction (D3/BDMA needs it pinned to
     * SRAM4) -- fine as long as only one BMP581 is ever in flight, which
     * matches the single I2C4 baro on this board. */
    static uint8_t rxBuffer_[6];
};

#endif /* BAROMETER_BAROMETER_HPP_ */
