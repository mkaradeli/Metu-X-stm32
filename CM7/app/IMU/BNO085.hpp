/*
 * BNO085.hpp
 *
 *  Created on: Jul 12, 2026
 *      Author: karadeli
 *
 * Bare-metal C++ driver for the BNO08x on STM32H7xx over SPI, built on the
 * CEVA SH-2 library (sh2.c / shtp.c / sh2_SensorValue.c / sh2_util.c).
 *
 * Fully interrupt- and DMA-driven transport. H_INTN (IMU_INT / PA10) is a
 * real EXTI15_10 falling-edge interrupt: the ISR starts the SHTP header
 * read via SPI+DMA the instant the hub asserts INTN. The header-complete
 * DMA callback (HAL_SPI_TxRxCpltCallback) chains straight into the body
 * read via DMA without ever returning to the main loop in between, so a
 * full packet -- header and body -- is captured by hardware/ISRs alone.
 * service() (via sh2_service() -> hal_read()) just checks a "packet
 * ready" flag; there is no GPIO polling and no HAL_SPI_GetState() polling
 * anywhere in the read path.
 *
 * Note on hal_write(): it stays a short, blocking (DMA-free) SPI
 * transfer. The CEVA sh2 HAL contract requires hal_write() to report
 * either the full byte count (sent) or 0 (nothing sent -- caller
 * retries) with no async/partial state in between, so an async write
 * would need to buffer and fake a synchronous-looking result anyway.
 * Writes are also rare, small (<=128 B) config/command packets -- not
 * the 400 Hz sensor stream -- so there's no throughput case for DMA
 * here, only correctness risk.
 *
 * Wiring/protocol (per the BNO08X datasheet):
 *  - IMU_INT (H_INTN, active low): the hub asserts this when it has data
 *    to send, or in response to a wake request. EXTI15_10, falling edge,
 *    pull-up (see gpio.c / stm32h7xx_it.c).
 *  - IMU_CS  (H_CSN, active low, software-controlled): asserted for the
 *    duration of each SPI transaction (from ISR context on the read
 *    path); the hub de-asserts INTN as soon as CS is observed.
 *  - IMU_RST (NRST, active low): hardware reset line.
 *  - IMU_P0  (PS0/WAKE): must be high from before reset until the first
 *    INTN assertion to select SPI mode (together with PS1, tied high on
 *    the PCB). Afterwards it is the WAKE line: pulsed low by the host to
 *    ask a sleeping hub to assert INTN so a write can be sent. Note: PS0
 *    has no external pull-up on this board, so it floats low whenever the
 *    STM32 isn't actively driving it (e.g. before MX_GPIO_Init() runs at
 *    boot) -- begin()/hal_open() always drives it high before toggling
 *    NRST, so SPI mode gets correctly re-selected on every begin() call
 *    regardless of what happened at board power-up.
 *  - SPI1, mode 3 (CPOL=1, CPHA=1), ~3 MHz (see spi.c / hspi1). DMA on
 *    both directions (DMA2 Stream0 TX / Stream1 RX), NSS software.
 *
 * service() must be called from the main loop, at least as often as the
 * shortest enabled report interval (packets are captured continuously in
 * the background regardless, but service()/sh2_service() is what drains
 * the "packet ready" flag and dispatches to sensor callbacks).
 *
 * The CEVA sh2 library keeps global state internally, so only ONE instance
 * of this class may exist. begin() enforces this.
 *
 * Build notes:
 *  - keep in build:   sh2.c, shtp.c, sh2_SensorValue.c, sh2_util.c
 *  - exclude:         old IMU.c, and do not include sh2_hal_init.h anywhere
 *                     (it pulls in FreeRTOS headers)
 *
 * Usage:
 *    BNO085 imu;
 *    if (!imu.begin(&hspi1)) { ... }
 *    imu.enableReport(SH2_GAME_ROTATION_VECTOR, 2500);   // 400 Hz
 *    imu.enableReport(SH2_RAW_ACCELEROMETER,  2500);
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

    // Resets the hub over SPI (NRST + PS0/WAKE + INTN handshake), opens the
    // sh2 session and reads the product IDs.
    // Returns false on failure (no INTN after reset / sh2_open error).
    bool begin(SPI_HandleTypeDef* hspi);

    void end();

    // Drains one pending SHTP packet if the background EXTI+DMA transport
    // has finished capturing one, and dispatches it via sh2_service().
    // Call from the main loop, at least as often as the shortest enabled
    // report interval. Also transparently re-enables all reports if the
    // hub reset itself (brown-out, watchdog).
    void service();

    // Enable a sensor report at the given interval (microseconds).
    // e.g. enableReport(SH2_GAME_ROTATION_VECTOR, 2500) -> 400 Hz.
    // The config is cached and re-applied automatically after a hub reset.
    bool enableReport(sh2_SensorId_t sensorId, uint32_t interval_us);

    bool disableReport(sh2_SensorId_t sensorId);

    // --- Latest data (updated inside service()) ---------------------------

    sh2_RotationVector_t     quaternion       {};   // SH2_GAME_ROTATION_VECTOR
    sh2_Accelerometer_t      linearAccel      {};   // SH2_LINEAR_ACCELERATION
    sh2_Accelerometer_t      accel            {};   // SH2_ACCELEROMETER
    sh2_RawAccelerometer_t   rawAccel         {};   // SH2_RAW_ACCELEROMETER (ADC counts, bypasses fusion engine)
    sh2_GyroIntegratedRV_t   gyroIntegratedRV {};   // SH2_GYRO_INTEGRATED_RV

    // Measured interval between consecutive reports (us), for diagnostics.
    uint32_t quaternionInterval_us    = 0;
    uint32_t accelInterval_us         = 0;
    uint32_t rawAccelInterval_us      = 0;
    uint32_t gyroIntegratedRVInterval_us = 0;

    // True once since the last time it was checked (read-and-clear).
    bool hasNewQuaternion();
    bool hasNewAccel();
    bool hasNewRawAccel();
    bool hasNewGyroIntegratedRV();

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
    uint32_t spiErrorCount() const;

    // Total number of hub resets (SH2_RESET async events) seen since
    // begin(), for watching in a live debugger view. Unlike wasReset(),
    // this never self-clears, so it shows whether the hub is
    // reset-looping rather than just "did it reset since I last asked."
    uint32_t resetCount = 0;

private:
    // sh2 C-callback trampolines
    static void sensorCallback(void* cookie, sh2_SensorEvent_t* event);
    static void asyncCallback(void* cookie, sh2_AsyncEvent_t* event);

    void handleSensorEvent(const sh2_SensorEvent_t* event);
    bool applyReportConfig(sh2_SensorId_t sensorId, uint32_t interval_us);
    void reapplyAllReports();

    SPI_HandleTypeDef* hspi_ = nullptr;

    bool open_ = false;

    struct ReportCfg {
        sh2_SensorId_t id;
        uint32_t interval_us;
        bool used;
    };
    ReportCfg reports_[kMaxReports] {};

    bool resetOccurred_       = false;
    bool userResetFlag_       = false;
    bool newQuaternion_       = false;
    bool newAccel_            = false;
    bool newRawAccel_         = false;
    bool newGyroIntegratedRV_ = false;
    bool newValue_            = false;

    uint32_t lastQuatStamp_us_     = 0;
    uint32_t lastAccelStamp_us_    = 0;
    uint32_t lastRawAccelStamp_us_ = 0;
    uint32_t lastGiRvStamp_us_     = 0;
};

#endif /* BNO085_HPP_ */
