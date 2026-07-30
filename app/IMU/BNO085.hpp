/*
 * BNO085.hpp
 *
 *  Created on: Jul 12, 2026
 *      Author: karadeli
 *
 * Bare-metal C++ driver for the BNO08x on STM32H7xx over I2C, built on the
 * CEVA SH-2 library (sh2.c / shtp.c / sh2_SensorValue.c / sh2_util.c).
 *
 * No RTOS. No INTN / RSTN wiring required (VCC, GND, SCL, SDA only):
 *  - reads are polled: call service() from the main loop, at least as fast
 *    as the shortest enabled report interval
 *  - reset is done in-band with an SHTP soft-reset packet
 *
 * The CEVA sh2 library keeps global state internally, so only ONE instance
 * of this class may exist. begin() enforces this.
 *
 * Build notes:
 *  - keep in build:   sh2.c, shtp.c, sh2_SensorValue.c, sh2_util.c
 *  - exclude:         spi_hal.c, old IMU.c, and do not include
 *                     sh2_hal_init.h anywhere (it pulls in FreeRTOS headers)
 *
 * Usage:
 *    BNO085 imu;
 *    if (!imu.begin(&hi2c1)) { ... }
 *    imu.enableReport(SH2_GAME_ROTATION_VECTOR, 2500);   // 400 Hz
 *    imu.enableReport(SH2_LINEAR_ACCELERATION,  2500);
 *    while (1) {
 *        imu.service();
 *        if (imu.hasNewQuaternion()) { auto q = imu.quaternion; ... }
 *    }
 */

#ifndef BNO085_HPP_
#define BNO085_HPP_

#include <cstdint>

#include "stm32h7xx_hal.h"

extern "C" {
#include "sh2.h"
#include "sh2_SensorValue.h"
#include "sh2_err.h"
}

class BNO085 {
public:
    // Maximum number of sensor reports that can be enabled at once
    // (they are cached so they can be re-applied after a hub reset).
    static constexpr int kMaxReports = 8;

    BNO085() = default;
    BNO085(const BNO085&) = delete;
    BNO085& operator=(const BNO085&) = delete;

    // Resets the hub (soft reset over I2C), opens the sh2 session and
    // reads the product IDs. addr7 is the 7-bit address: 0x4A (SA0 low,
    // default on most breakouts) or 0x4B (SA0 high).
    // Returns false on failure (no ACK / sh2_open error).
    bool begin(I2C_HandleTypeDef* hi2c, uint8_t addr7 = 0x4A);

    void end();

    // Pump the sh2 library: polls the device for pending packets and
    // dispatches sensor callbacks. Call from the main loop, at least as
    // often as the shortest enabled report interval. Also transparently
    // re-enables all reports if the hub reset itself (brown-out, watchdog).
    void service();

    // Enable a sensor report at the given interval (microseconds).
    // e.g. enableReport(SH2_GAME_ROTATION_VECTOR, 2500) -> 400 Hz.
    // The config is cached and re-applied automatically after a hub reset.
    bool enableReport(sh2_SensorId_t sensorId, uint32_t interval_us);

    bool disableReport(sh2_SensorId_t sensorId);

    // --- Latest data (updated inside service()) ---------------------------

    sh2_RotationVector_t quaternion  {};   // SH2_GAME_ROTATION_VECTOR
    sh2_Accelerometer_t  linearAccel {};   // SH2_LINEAR_ACCELERATION

    // Measured interval between consecutive reports (us), for diagnostics.
    uint32_t quaternionInterval_us = 0;
    uint32_t accelInterval_us      = 0;

    // True once since the last time it was checked (read-and-clear).
    bool hasNewQuaternion();
    bool hasNewAccel();

    // Any other enabled sensor: latest decoded value + read-and-clear flag.
    // Check hasNewValue() first, then read lastValue.
    bool hasNewValue();
    sh2_SensorValue_t lastValue {};

    // True if the hub reset itself since the last call (read-and-clear).
    // service() already re-applies the report configs; this is informative.
    bool wasReset();

    // Filled by begin()
    sh2_ProductIds_t prodIds {};

    // Diagnostics
    uint32_t rxPacketCount() const;
    uint32_t i2cErrorCount() const;

private:
    // sh2 C-callback trampolines
    static void sensorCallback(void* cookie, sh2_SensorEvent_t* event);
    static void asyncCallback(void* cookie, sh2_AsyncEvent_t* event);

    void handleSensorEvent(const sh2_SensorEvent_t* event);
    bool applyReportConfig(sh2_SensorId_t sensorId, uint32_t interval_us);
    void reapplyAllReports();

    I2C_HandleTypeDef* hi2c_ = nullptr;
    uint16_t addr8_ = 0;            // HAL-style shifted address

    bool open_ = false;

    struct ReportCfg {
        sh2_SensorId_t id;
        uint32_t interval_us;
        bool used;
    };
    ReportCfg reports_[kMaxReports] {};

    volatile bool resetOccurred_   = false;
    volatile bool userResetFlag_   = false;
    volatile bool newQuaternion_   = false;
    volatile bool newAccel_        = false;
    volatile bool newValue_        = false;

    uint32_t lastQuatStamp_us_  = 0;
    uint32_t lastAccelStamp_us_ = 0;
};

#endif /* BNO085_HPP_ */
