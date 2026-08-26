/*
 * BNO085.cpp
 *
 *  Created on: Jul 12, 2026
 *      Author: karadeli
 *
 * Implementation. The CEVA sh2_Hal_t transport is implemented here as
 * file-local static functions.
 *
 * Read path -- fully EXTI + DMA driven, no polling anywhere:
 *   1. IMU_INT (H_INTN) falling edge -> HAL_GPIO_EXTI_Callback() -> if the
 *      bus is idle, asserts CS and starts a 4-byte SHTP header read via
 *      HAL_SPI_TransmitReceive_DMA(), right there in interrupt context.
 *   2. Header DMA completes -> HAL_SPI_TxRxCpltCallback() -> parses the
 *      SHTP length field; if there's a body, starts the body DMA read
 *      immediately (same CS window -- SPI has no restart condition), all
 *      still in interrupt context. No trip back through the main loop
 *      between header and body: that was tried once (see git history,
 *      commit 6f80e35) and reverted because the extra service()
 *      round-trip pushed fusion-engine-gated reports like
 *      SH2_ACCELEROMETER into throttled territory. Chaining inside the
 *      ISR avoids that regression entirely.
 *   3. Body DMA completes -> HAL_SPI_TxRxCpltCallback() -> deasserts CS,
 *      sets s_packetReady. hal_read(), called from sh2_service() in the
 *      main loop, is now just "if (!s_packetReady) return 0；else memcpy
 *      and return len" -- no GPIO read, no HAL_SPI_GetState() poll.
 *
 * A single RX buffer is used (no double-buffering): tryStartHeaderRead()
 * refuses to start a new header read until hal_read() has drained the
 * previous packet, so a still-unread packet is never clobbered. At the
 * enabled report rates (<=1 kHz) this never becomes a bottleneck -- a
 * full header+body transfer at ~3 MHz is on the order of tens of
 * microseconds, several orders of magnitude below the report period.
 *
 * A software timeout watchdog (checked from hal_read(), i.e. every
 * service() tick) replaces the old polling loop's timeout: since nothing
 * polls the SPI peripheral state anymore, a wedged transfer (e.g. a
 * noise glitch that doesn't raise a HAL error IT) needs an independent
 * way to get noticed and recovered.
 *
 * Write path -- stays a short, blocking (DMA-free) transfer: see
 * BNO085.hpp for why. It still uses the ISR-maintained INTN flag instead
 * of a raw GPIO read, and it takes the same s_phase gate as the read
 * path so the two can never touch the bus at the same time.
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

// Timeout for a single blocking SPI transfer (hal_write()), and separately
// the watchdog window for a background header/body DMA transfer stuck in
// flight (checked from hal_read()).
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

enum class XferPhase : uint8_t { Idle, Header, Body, Write };

// s_phase and s_intnAsserted/s_packetReady are touched from EXTI and SPI
// DMA ISR context as well as from hal_read()/hal_write() (thread/main-loop
// context) -- every access outside of a __disable_irq()/__enable_irq()
// section, or that isn't a single-flag ISR->thread handoff, must go
// through one of the helpers below.
static volatile XferPhase s_phase        = XferPhase::Idle;
static volatile bool      s_intnAsserted = false;  // ISR-maintained mirror of the H_INTN level
static volatile bool      s_packetReady  = false;  // header+body DMA done; s_rxBuf[0..s_pendingLen) valid
static volatile uint16_t  s_pendingLen   = 0;       // total packet length (header+body)
static volatile uint32_t  s_pendingStamp = 0;       // INTN-observed timestamp for this packet
static volatile uint32_t  s_xferStartTick = 0;      // HAL_GetTick() when the current phase started

__attribute__((section(".sram3"), aligned(32), used))
static uint8_t s_rxBuf[kRxBufSize];
// Dummy all-zero MOSI data clocked out while reading (header or body).
__attribute__((section(".sram3"), aligned(32), used))
static uint8_t s_txZeros[kRxBufSize];

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

// H_INTN is active low. Raw GPIO read -- used only by hal_open()'s
// one-time post-reset handshake wait, before the transport's normal
// ISR-driven s_intnAsserted flag has any meaning to wait on. Everywhere
// else in this file uses s_intnAsserted instead.
static inline bool intnAssertedRaw()
{
    return HAL_GPIO_ReadPin(IMU_INT_GPIO_Port, IMU_INT_Pin) == GPIO_PIN_RESET;
}

// ---------------------------------------------------------------------------
// Read-path state machine (EXTI ISR + SPI DMA completion ISR + hal_read())
// ---------------------------------------------------------------------------

// Starts a header DMA read if the bus is idle, INTN is (still) asserted,
// and the previous packet has already been drained by hal_read(). Safe to
// call from ISR or thread context. No-op otherwise.
static void tryStartHeaderRead()
{
    bool go;
    __disable_irq();
    go = (s_hspi != nullptr) && (s_phase == XferPhase::Idle) &&
         s_intnAsserted && !s_packetReady;
    if (go) {
        s_phase = XferPhase::Header;
        s_pendingStamp = timeNowUs();
        s_xferStartTick = HAL_GetTick();
        csAssert(true);
        s_intnAsserted = false;   // hub drops INTN once it sees CS low
    }
    __enable_irq();

    if (!go) {
        return;
    }

    if (HAL_SPI_TransmitReceive_DMA(s_hspi, s_txZeros, s_rxBuf, kReadLen) != HAL_OK) {
        csAssert(false);
        s_phase = XferPhase::Idle;
        ++s_spiErrCount;
    }
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin != IMU_INT_Pin) {
        return;
    }
    s_intnAsserted = true;
    tryStartHeaderRead();
}

extern "C" void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi)
{
    if (hspi->Instance != SPI1) {
        return;
    }

    if (s_phase == XferPhase::Header) {
        uint16_t pktLen = (uint16_t)(s_rxBuf[0] | (s_rxBuf[1] << 8)) & (uint16_t)~0x8000;
        if (pktLen > kRxBufSize) {
            pktLen = (uint16_t)kRxBufSize;
        }

        if (pktLen <= kReadLen) {
            // Header-only packet: nothing to deliver.
            csAssert(false);
            s_phase = XferPhase::Idle;
            tryStartHeaderRead();   // in case INTN is already back up for a new packet
            return;
        }

        const uint16_t bodyLen = pktLen - kReadLen;
        s_pendingLen = pktLen;
        s_phase = XferPhase::Body;
        s_xferStartTick = HAL_GetTick();
        if (HAL_SPI_TransmitReceive_DMA(s_hspi, s_txZeros, s_rxBuf + kReadLen, bodyLen) != HAL_OK) {
            csAssert(false);
            s_phase = XferPhase::Idle;
            ++s_spiErrCount;
            tryStartHeaderRead();
        }
        return;
    }

    if (s_phase == XferPhase::Body) {
        csAssert(false);
        s_phase = XferPhase::Idle;
        ++s_rxPacketCount;
        s_packetReady = true;   // hal_read() drains this and re-arms the next read
        return;
    }

    // XferPhase::Write never reaches here -- hal_write() uses the
    // blocking (non-DMA) HAL_SPI_TransmitReceive(), which doesn't trigger
    // this callback.
}

extern "C" void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi)
{
    if (hspi->Instance != SPI1) {
        return;
    }
    ++s_spiErrCount;
    csAssert(false);
    s_phase = XferPhase::Idle;
    tryStartHeaderRead();
}

// ---------------------------------------------------------------------------
// sh2 HAL methods
// ---------------------------------------------------------------------------

static int hal_open(sh2_Hal_t* /*self*/)
{
    __disable_irq();
    s_phase = XferPhase::Idle;
    s_intnAsserted = false;
    s_packetReady = false;
    __enable_irq();

    std::memset(s_rxBuf, 0, sizeof(s_rxBuf));
    std::memset(s_txZeros, 0, sizeof(s_txZeros));

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
    while (!intnAssertedRaw()) {
        if ((HAL_GetTick() - start) > kResetTimeoutMs) {
            return SH2_ERR_IO;   // no INTN: wiring / power?
        }
    }

    return SH2_OK;
}

static void hal_close(sh2_Hal_t* /*self*/)
{
    __disable_irq();
    s_phase = XferPhase::Idle;
    __enable_irq();

    (void)HAL_SPI_Abort(s_hspi);
    csAssert(false);
    rstRelease(false);   // hold hub in reset

    s_intnAsserted = false;
    s_packetReady = false;
}

// Non-blocking read: the header+body transfer already happened in the
// background (EXTI ISR -> DMA -> DMA completion ISR, see above). This
// just checks whether a packet is ready and, if so, hands it over.
static int hal_read(sh2_Hal_t* /*self*/, uint8_t* pBuffer, unsigned len,
                    uint32_t* t)
{
    // Watchdog: recover a transfer that's been stuck mid-flight for too
    // long (e.g. an SPI noise glitch that never raised a HAL error IT).
    // Nothing else polls the peripheral state anymore, so this has to.
    XferPhase phase;
    uint32_t startTick;
    __disable_irq();
    phase = s_phase;
    startTick = s_xferStartTick;
    __enable_irq();

    if ((phase != XferPhase::Idle) && (phase != XferPhase::Write) &&
        ((HAL_GetTick() - startTick) > kSpiTimeoutMs)) {
        (void)HAL_SPI_Abort(s_hspi);
        csAssert(false);
        s_phase = XferPhase::Idle;
        s_packetReady = false;
        ++s_spiErrCount;
        tryStartHeaderRead();
    }

    if (!s_packetReady) {
        return 0;
    }

    __disable_irq();
    uint16_t n = s_pendingLen;
    const uint32_t stamp = s_pendingStamp;
    s_packetReady = false;
    __enable_irq();

    if (n > len) {
        n = (uint16_t)len;   // shouldn't happen: sh2 passes MAX_TRANSFER_IN
    }
    std::memcpy(pBuffer, s_rxBuf, n);
    *t = stamp;

    tryStartHeaderRead();   // buffer is free again; chain the next packet if pending
    return n;
}

// Short blocking write (see BNO085.hpp for why this isn't DMA/async):
// wake the hub if it isn't already signaling INTN, then transmit within a
// CS-low window. Whatever comes back on MISO during the write is
// discarded -- the next background read will pick up anything real the
// hub still has pending.
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

    bool busy;
    __disable_irq();
    busy = (s_phase != XferPhase::Idle);
    if (!busy) {
        s_phase = XferPhase::Write;
    }
    __enable_irq();
    if (busy) {
        return 0;   // header/body read (or another write) in flight; sh2 retries
    }

    if (!s_intnAsserted) {
        wakeIdle(false);   // assert WAKE

        const uint32_t start = HAL_GetTick();
        bool woke = false;
        while ((HAL_GetTick() - start) <= kWakeTimeoutMs) {
            if (s_intnAsserted) {
                woke = true;
                break;
            }
        }
        wakeIdle(true);
        if (!woke) {
            s_phase = XferPhase::Idle;
            return 0;   // hub didn't respond; sh2 lib retries
        }
    }

    csAssert(true);
    s_intnAsserted = false;   // hub drops INTN once it sees CS low

    uint8_t discard[kTxBufSize];
    const bool ok = (HAL_SPI_TransmitReceive(s_hspi, pBuffer, discard, (uint16_t)len,
                                              kSpiTimeoutMs) == HAL_OK);
    csAssert(false);
    s_phase = XferPhase::Idle;

    if (!ok) {
        ++s_spiErrCount;
        return 0;
    }

    tryStartHeaderRead();   // in case INTN came back up during/after the write
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
