/*
 * BNO085.cpp
 *
 *  Created on: Jul 12, 2026
 *      Author: karadeli
 *
 * Implementation. The CEVA sh2_Hal_t transport is implemented here as
 * file-local static functions, purely polled and blocking: hal_read()
 * checks the INTN pin's level directly and, if asserted, performs the
 * whole SPI transfer synchronously with HAL_SPI_TransmitReceive(). No
 * EXTI interrupt, no DMA, no background state machine -- so there is
 * nothing async that can get wedged between service() calls.
 */

#include "app_main.hpp"         // micros()
#include "main.h"                // IMU_CS_Pin, IMU_RST_Pin, IMU_INT_Pin, IMU_P0_Pin
#include "BNO085.hpp"

#include <cstring>

extern "C" {
#include "sh2_hal.h"     // sh2_Hal_t, SH2_HAL_MAX_TRANSFER_*
}

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

// How long to wait for the hub to assert INTN after reset (t1+t2 ~= 94 ms
// typical per the datasheet; generous margin for cold starts).
static constexpr uint32_t kResetTimeoutMs = 500;

// How long to wait for INTN after asserting WAKE for a write (twk is
// ~150 us typical per the datasheet; generous margin).
static constexpr uint32_t kWakeTimeoutMs = 10;

// Timeout for a single blocking SPI transfer.
static constexpr uint32_t kSpiTimeoutMs = 20;

// How many bytes to read when reading the SHTP length header.
static constexpr unsigned kReadLen = 4;

// ---------------------------------------------------------------------------
// File-local transport state
// ---------------------------------------------------------------------------

static SPI_HandleTypeDef* s_hspi = nullptr;

static uint32_t s_rxPacketCount = 0;
static uint32_t s_spiErrCount   = 0;

static constexpr size_t kRxBufSize = SH2_HAL_MAX_TRANSFER_IN;
static constexpr size_t kTxBufSize = SH2_HAL_MAX_TRANSFER_OUT;

static uint8_t s_rxBuf[kRxBufSize];
// Dummy all-zero MOSI data clocked out while reading (header or body).
static uint8_t s_txZeros[kRxBufSize] = {0};

static uint32_t timeNowUs()
{
    return micros();
}

// ---------------------------------------------------------------------------
// GPIO helpers
// ---------------------------------------------------------------------------

static inline void csAssert(bool asserted)
{
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin,
                       asserted ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static inline void rstRelease(bool released)
{
    HAL_GPIO_WritePin(IMU_RST_GPIO_Port, IMU_RST_Pin,
                       released ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// PS0/WAKE: idle high, driven low to request the hub wake up / assert INTN.
static inline void wakeIdle(bool idle)
{
    HAL_GPIO_WritePin(IMU_P0_GPIO_Port, IMU_P0_Pin,
                       idle ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// H_INTN is active low.
static inline bool intnAsserted()
{
    return HAL_GPIO_ReadPin(IMU_INT_GPIO_Port, IMU_INT_Pin) == GPIO_PIN_RESET;
}

// ---------------------------------------------------------------------------
// sh2 HAL methods
// ---------------------------------------------------------------------------

static int hal_open(sh2_Hal_t* /*self*/)
{
    // Hold the hub in reset while lines settle.
    rstRelease(false);
    csAssert(false);

    // PS0/WAKE (and PS1, tied high on the PCB) must be high from before
    // reset until the first INTN assertion to select SPI mode. PS0 has no
    // external pull-up, so driving it here (rather than relying on any
    // power-up default) is what makes SPI mode selection reliable.
    wakeIdle(true);

    HAL_Delay(1);        // >> tnrst (10 ns min)

    rstRelease(true);    // release reset, hub begins booting

    const uint32_t start = HAL_GetTick();
    while (!intnAsserted()) {
        if ((HAL_GetTick() - start) > kResetTimeoutMs) {
            return SH2_ERR_IO;   // no INTN: wiring / power?
        }
    }

    return SH2_OK;
}

static void hal_close(sh2_Hal_t* /*self*/)
{
    csAssert(false);
    rstRelease(false);   // hold hub in reset
}

// Polled, blocking read: if INTN isn't asserted, there's nothing pending.
// If it is, read the 4-byte header, then (if there's more) the body, all
// within one CS-low window, and hand the whole packet back.
static int hal_read(sh2_Hal_t* /*self*/, uint8_t* pBuffer, unsigned len,
                    uint32_t* t)
{
    if (!intnAsserted()) {
        return 0;
    }

    const uint32_t rxStamp = timeNowUs();

    csAssert(true);

    if (HAL_SPI_TransmitReceive(s_hspi, s_txZeros, s_rxBuf, kReadLen,
                                 kSpiTimeoutMs) != HAL_OK) {
        ++s_spiErrCount;
        csAssert(false);
        return 0;
    }

    uint16_t pktLen = (uint16_t)(s_rxBuf[0] | (s_rxBuf[1] << 8)) & (uint16_t)~0x8000;
    if (pktLen > kRxBufSize) {
        pktLen = (uint16_t)kRxBufSize;
    }

    if (pktLen > kReadLen) {
        // More to read: continue clocking within the same CS window
        // (SPI has no restart condition, so the body follows directly).
        const uint16_t bodyLen = pktLen - kReadLen;
        if (HAL_SPI_TransmitReceive(s_hspi, s_txZeros, s_rxBuf + kReadLen,
                                     bodyLen, kSpiTimeoutMs) != HAL_OK) {
            ++s_spiErrCount;
            csAssert(false);
            return 0;
        }
    }

    csAssert(false);

    if (pktLen <= kReadLen) {
        return 0;   // header only; nothing pending
    }

    ++s_rxPacketCount;

    uint16_t n = pktLen;
    if (n > len) {
        n = (uint16_t)len;   // shouldn't happen: sh2 passes MAX_TRANSFER_IN
    }
    std::memcpy(pBuffer, s_rxBuf, n);
    *t = rxStamp;
    return n;
}

// Blocking write: wake the hub if it isn't already signaling INTN, then
// transmit within a CS-low window. Whatever comes back on MISO during the
// write is discarded -- the next hal_read() call will pick up anything
// real the hub still has pending.
static int hal_write(sh2_Hal_t* /*self*/, uint8_t* pBuffer, unsigned len)
{
    if ((len > 0) && (pBuffer == nullptr)) {
        return SH2_ERR_BAD_PARAM;
    }
    if (len == 0) {
        return 0;
    }
    if (len > kTxBufSize) {
        return SH2_ERR_BAD_PARAM;
    }

    if (!intnAsserted()) {
        wakeIdle(false);   // assert WAKE

        const uint32_t start = HAL_GetTick();
        while (!intnAsserted()) {
            if ((HAL_GetTick() - start) > kWakeTimeoutMs) {
                wakeIdle(true);
                return 0;   // hub didn't respond; sh2 lib retries
            }
        }
        wakeIdle(true);
    }

    csAssert(true);

    uint8_t discard[kTxBufSize];
    if (HAL_SPI_TransmitReceive(s_hspi, pBuffer, discard, (uint16_t)len,
                                 kSpiTimeoutMs) != HAL_OK) {
        ++s_spiErrCount;
        csAssert(false);
        return 0;
    }

    csAssert(false);
    return (int)len;
}

static uint32_t hal_getTimeUs(sh2_Hal_t* /*self*/)
{
    return timeNowUs();
}

// Field order in sh2_Hal_t: open, close, read, write, getTimeUs
static sh2_Hal_t s_hal = {
    hal_open,
    hal_close,
    hal_read,
    hal_write,
    hal_getTimeUs,
};

// ---------------------------------------------------------------------------
// BNO085 class
// ---------------------------------------------------------------------------

bool BNO085::begin(SPI_HandleTypeDef* hspi)
{
    if ((s_hspi != nullptr) && !open_) {
        // Another instance has the sh2 library open: it keeps global state
        // internally, so only one open at a time.
        return false;
    }
    if (hspi == nullptr) {
        return false;
    }

    hspi_  = hspi;
    s_hspi = hspi_;

    // Opens the transport (hardware reset inside hal_open) and processes
    // the hub's startup traffic. Blocks internally until done.
    if (sh2_open(&s_hal, asyncCallback, this) != SH2_OK) {
        s_hspi = nullptr;
        return false;
    }

    open_ = true;
    resetOccurred_ = false;

    std::memset(reports_, 0, sizeof(reports_));

    sh2_setSensorCallback(sensorCallback, this);

    // Product IDs: cheap sanity check that two-way traffic really works.
    std::memset(&prodIds, 0, sizeof(prodIds));
    (void)sh2_getProdIds(&prodIds);

    return true;
}

void BNO085::end()
{
    if (open_) {
        sh2_close();
        open_ = false;
    }
    s_hspi = nullptr;
}

void BNO085::service()
{
    if (!open_) {
        return;
    }

    sh2_service();

    // Hub reset (brown-out, watchdog, firmware fault): all report configs
    // are lost -- re-apply them here, outside of any callback.
    if (resetOccurred_) {
        resetOccurred_ = false;
        reapplyAllReports();
        userResetFlag_ = true;
    }
}

bool BNO085::enableReport(sh2_SensorId_t sensorId, uint32_t interval_us)
{
    if (!open_) {
        return false;
    }

    // Cache (or update) the config so it survives hub resets.
    ReportCfg* slot = nullptr;
    for (auto& r : reports_) {
        if (r.used && r.id == sensorId) { slot = &r; break; }
    }
    if (slot == nullptr) {
        for (auto& r : reports_) {
            if (!r.used) { slot = &r; break; }
        }
    }
    if (slot == nullptr) {
        return false;   // kMaxReports exceeded
    }

    slot->id          = sensorId;
    slot->interval_us = interval_us;
    slot->used        = true;

    return applyReportConfig(sensorId, interval_us);
}

bool BNO085::disableReport(sh2_SensorId_t sensorId)
{
    if (!open_) {
        return false;
    }
    for (auto& r : reports_) {
        if (r.used && r.id == sensorId) {
            r.used = false;
        }
    }
    return applyReportConfig(sensorId, 0);   // interval 0 disables
}

bool BNO085::applyReportConfig(sh2_SensorId_t sensorId, uint32_t interval_us)
{
    sh2_SensorConfig_t config = {};
    config.reportInterval_us = interval_us;
    return sh2_setSensorConfig(sensorId, &config) == SH2_OK;
}

void BNO085::reapplyAllReports()
{
    for (const auto& r : reports_) {
        if (r.used) {
            (void)applyReportConfig(r.id, r.interval_us);
        }
    }
}

bool BNO085::hasNewQuaternion()
{
    const bool v = newQuaternion_;
    newQuaternion_ = false;
    return v;
}

bool BNO085::hasNewAccel()
{
    const bool v = newAccel_;
    newAccel_ = false;
    return v;
}

bool BNO085::hasNewRawAccel()
{
    const bool v = newRawAccel_;
    newRawAccel_ = false;
    return v;
}

bool BNO085::hasNewGyroIntegratedRV()
{
    const bool v = newGyroIntegratedRV_;
    newGyroIntegratedRV_ = false;
    return v;
}

bool BNO085::hasNewValue()
{
    const bool v = newValue_;
    newValue_ = false;
    return v;
}

bool BNO085::wasReset()
{
    const bool v = userResetFlag_;
    userResetFlag_ = false;
    return v;
}

uint32_t BNO085::rxPacketCount() const { return s_rxPacketCount; }
uint32_t BNO085::spiErrorCount() const { return s_spiErrCount; }

// --- sh2 callback plumbing -------------------------------------------------

void BNO085::sensorCallback(void* cookie, sh2_SensorEvent_t* event)
{
    static_cast<BNO085*>(cookie)->handleSensorEvent(event);
}

void BNO085::asyncCallback(void* cookie, sh2_AsyncEvent_t* event)
{
    auto* self = static_cast<BNO085*>(cookie);
    if (event->eventId == SH2_RESET) {
        self->resetOccurred_ = true;
        ++self->resetCount;
    }
}

void BNO085::handleSensorEvent(const sh2_SensorEvent_t* event)
{
    sh2_SensorValue_t value;
    if (sh2_decodeSensorEvent(&value, event) != SH2_OK) {
        return;
    }

    const uint32_t now = timeNowUs();
    // Hub-reported, delay-compensated sample time (sh2_decodeSensorEvent()
    // sets this from the SHTP receive timestamp minus the report's own
    // internal delay), truncated to 32 bits -- intervals are microseconds
    // apart, far below the 32-bit wraparound period.
    const uint32_t stamp = (uint32_t)value.timestamp;

    switch (value.sensorId) {
    case SH2_LINEAR_ACCELERATION:
        linearAccel = value.un.linearAcceleration;
        accelInterval_us = stamp - lastAccelStamp_us_;
        lastAccelStamp_us_ = stamp;
        newAccel_ = true;
        break;
    case SH2_ACCELEROMETER:
    	accel = value.un.accelerometer;
    	accelInterval_us = stamp - lastAccelStamp_us_;
		lastAccelStamp_us_ = stamp;
		newAccel_ = true;
		break;
    case SH2_RAW_ACCELEROMETER:
        // ADC counts, straight from the sensor -- bypasses the hub's
        // fusion/calibration engine entirely, unlike SH2_ACCELEROMETER.
        rawAccel = value.un.rawAccelerometer;
        rawAccelInterval_us = stamp - lastRawAccelStamp_us_;
        lastRawAccelStamp_us_ = stamp;
        newRawAccel_ = true;
        break;
    case SH2_GAME_ROTATION_VECTOR:
        quaternion = value.un.gameRotationVector;
        quaternionInterval_us = now - lastQuatStamp_us_;
        lastQuatStamp_us_ = now;
        newQuaternion_ = true;
        break;

    case SH2_GYRO_INTEGRATED_RV:
        // Quaternion (real,i,j,k) + integrated angular velocity
        // (angVelX/Y/Z, rad/s) in one report. Produced by the hub's
        // gyro-integrator, so it can run much faster than the fused
        // game-rotation-vector (up to ~1 kHz).
        gyroIntegratedRV = value.un.gyroIntegratedRV;
        gyroIntegratedRVInterval_us = stamp - lastGiRvStamp_us_;
        lastGiRvStamp_us_ = stamp;
        newGyroIntegratedRV_ = true;
        break;

    default:
        lastValue = value;
        newValue_ = true;
        break;
    }
}
