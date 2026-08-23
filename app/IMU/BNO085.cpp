/*
 * BNO085.cpp
 *
 *  Created on: Jul 12, 2026
 *      Author: karadeli
 *
 * Implementation. The CEVA sh2_Hal_t transport is implemented here as
 * file-local static functions on top of SPI1 + DMA, so sh2_hal_init.h /
 * the old CEVA spi_hal.c are not needed at all.
 *
 * State machine (spiActivate/spiCompleted) ported from CEVA's reference
 * SPI HAL (sh2-demo-nucleo/app/spi_hal.c), swapping HAL_SPI_TransmitReceive_IT
 * for the DMA-based transfer already wired up for hspi1 in spi.c.
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

// DMA buffer placement/alignment: RAM_D2 is configured non-cacheable by the
// MPU (see MPU_Config() in main.c, region 0x30000000/512K), so no cache
// maintenance is needed for buffers placed here -- they just need to be in
// memory DMA2 can reach (DTCM is not DMA-accessible at all).
#ifndef BNO085_DMA_BUF_ATTR
#define BNO085_DMA_BUF_ATTR __attribute__((section(".RAM_D2_Section")))
#endif

// ---------------------------------------------------------------------------
// File-local transport state
// ---------------------------------------------------------------------------

enum class SpiState : uint8_t { Idle, RdHdr, RdBody, Write };

// How many bytes to read when reading the SHTP length header
static constexpr unsigned kReadLen = 4;

static BNO085* s_active = nullptr;
static SPI_HandleTypeDef* s_hspi = nullptr;

static volatile SpiState s_state = SpiState::Idle;

// true from the time INTN is observed until a transfer is started for it
static volatile bool s_rxReady = false;

// true from the time the hub is put in reset until the first INTN
static volatile bool s_inReset = false;

static volatile uint32_t s_rxTimestamp_us = 0;

// >0 once a full SHTP transfer is sitting in s_rxBuf, ready for hal_read()
static volatile uint32_t s_rxBufLen = 0;

// >0 while a write is queued, waiting for the hub to wake up
static volatile uint32_t s_txBufLen = 0;

static volatile uint32_t s_rxPacketCount = 0;
static volatile uint32_t s_spiErrCount   = 0;

// Tick (ms) at which the current non-Idle transfer was started, and how
// long to wait before assuming it's wedged. A full 1024-byte transfer at
// 3 MHz takes ~2.7 ms; this leaves a wide margin for scheduling jitter.
static volatile uint32_t s_transferStartTick = 0;
static constexpr uint32_t kTransferTimeoutMs = 20;

// 32-byte aligned AND padded to a multiple of 32, matching the original I2C
// buffer sizing (cosmetic here since RAM_D2 isn't cached, but keeps DMA
// transfers on tidy boundaries).
static constexpr size_t kRxBufSize =
    ((SH2_HAL_MAX_TRANSFER_IN + 31u) / 32u) * 32u;
static constexpr size_t kTxBufSize =
    ((SH2_HAL_MAX_TRANSFER_OUT + 31u) / 32u) * 32u;

BNO085_DMA_BUF_ATTR static uint8_t s_rxBuf[kRxBufSize];
BNO085_DMA_BUF_ATTR static uint8_t s_txBuf[kTxBufSize];
// Dummy all-zero MOSI data clocked out while reading (header or body).
// Not const: a const array placed in the same named section as the
// writable buffers above causes a section-type conflict at link time.
BNO085_DMA_BUF_ATTR static uint8_t s_txZeros[kRxBufSize] = {0};

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

// Scoped critical section around the shared state machine: only the
// transport's own IRQ sources are masked, so the rest of the system's
// real-time interrupts (ADC/timer ISRs etc.) are never touched.
static inline void criticalEnter()
{
    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
    HAL_NVIC_DisableIRQ(DMA2_Stream0_IRQn);
    HAL_NVIC_DisableIRQ(DMA2_Stream1_IRQn);
}

static inline void criticalExit()
{
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// ---------------------------------------------------------------------------
// SPI bus state machine (ISR/DMA-callback context, mirrors CEVA's
// reference spi_hal.c spiActivate()/spiCompleted())
// ---------------------------------------------------------------------------

// Starts a DMA transfer and records when it started (for the watchdog in
// service()). Returns false if the HAL call itself failed to start a
// transfer -- in that case no completion callback will ever arrive, so the
// caller must not leave s_state pointing at a phase with nothing in flight.
static bool spiStart(const uint8_t* tx, uint8_t* rx, uint16_t len)
{
    if (HAL_SPI_TransmitReceive_DMA(s_hspi, tx, rx, len) != HAL_OK) {
        ++s_spiErrCount;
        csAssert(false);
        s_state = SpiState::Idle;
        return false;
    }
    s_transferStartTick = HAL_GetTick();
    return true;
}

// Attempt to start a SPI operation. Called from EXTI (INTN) context, from
// DMA-completion context (to chain the next phase or start a queued op),
// and from hal_read()/hal_write() (inside a critical section) once the bus
// is free.
static void spiActivate()
{
    if ((s_state == SpiState::Idle) && (s_rxBufLen == 0)) {
        if (s_rxReady) {
            s_rxReady = false;

            csAssert(true);

            if (s_txBufLen > 0) {
                s_state = SpiState::Write;
                if (spiStart(s_txBuf, s_rxBuf, (uint16_t)s_txBufLen)) {
                    wakeIdle(true);   // release WAKE now that CS has it covered
                }
            } else {
                s_state = SpiState::RdHdr;
                spiStart(s_txZeros, s_rxBuf, kReadLen);
            }
        }
    }
}

// Handle the end of a SPI DMA operation. May start a follow-up operation
// (header -> body) or return to idle, in which case it tries to start the
// next queued operation via spiActivate().
static void spiCompleted()
{
    uint16_t rxLen = (uint16_t)(s_rxBuf[0] | (s_rxBuf[1] << 8)) & (uint16_t)~0x8000;
    if (rxLen > kRxBufSize) {
        rxLen = (uint16_t)kRxBufSize;
    }

    switch (s_state) {
    case SpiState::RdHdr:
        if (rxLen > kReadLen) {
            // More to read: continue clocking within the same CS window
            // (SPI has no restart condition, so the body follows directly).
            s_state = SpiState::RdBody;
            if (!spiStart(s_txZeros, s_rxBuf + kReadLen,
                           (uint16_t)(rxLen - kReadLen))) {
                spiActivate();   // nothing in flight now; try the next op
            }
        } else {
            // No SHTP payload beyond the header; done.
            csAssert(false);
            s_rxBufLen = 0;
            s_state = SpiState::Idle;
            spiActivate();
        }
        break;

    case SpiState::RdBody:
        csAssert(false);
        s_rxBufLen = rxLen;
        s_state = SpiState::Idle;
        ++s_rxPacketCount;
        spiActivate();
        break;

    case SpiState::Write:
        csAssert(false);
        // Transaction only covered txBufLen bytes, so at most that many
        // bytes of whatever the hub sent back are valid.
        s_rxBufLen = (s_txBufLen < rxLen) ? s_txBufLen : rxLen;
        s_txBufLen = 0;
        s_state = SpiState::Idle;
        if (s_rxBufLen > 0) {
            ++s_rxPacketCount;
        }
        spiActivate();
        break;

    case SpiState::Idle:
    default:
        break;
    }
}

// Safety net for whatever spiStart()'s HAL_OK check doesn't catch (e.g. a
// DMA transfer that HAL reports as started but whose completion callback
// never arrives). Called every service() tick from application context;
// if a transfer has been in flight too long, force the bus back to a known
// state so the driver keeps running instead of staying wedged for the rest
// of the flight.
static void spiWatchdog()
{
    if (s_state == SpiState::Idle) {
        return;
    }
    if ((HAL_GetTick() - s_transferStartTick) <= kTransferTimeoutMs) {
        return;
    }

    criticalEnter();
    if (s_state != SpiState::Idle) {
        (void)HAL_SPI_Abort(s_hspi);
        csAssert(false);
        ++s_spiErrCount;
        s_state    = SpiState::Idle;
        s_rxBufLen = 0;
        s_txBufLen = 0;
        spiActivate();   // in case rxReady is already pending
    }
    criticalExit();
}

// ---------------------------------------------------------------------------
// HAL callbacks (ISR context)
// ---------------------------------------------------------------------------

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin != IMU_INT_Pin) {
        return;
    }
    if (s_active == nullptr) {
        return;
    }

    s_rxTimestamp_us = timeNowUs();
    s_inReset = false;
    s_rxReady = true;

    spiActivate();
}

extern "C" void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi)
{
    if ((s_hspi == nullptr) || (hspi->Instance != s_hspi->Instance)) {
        return;
    }
    if (s_active == nullptr) {
        return;
    }
    spiCompleted();
}

extern "C" void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi)
{
    if ((s_hspi == nullptr) || (hspi->Instance != s_hspi->Instance)) {
        return;
    }
    ++s_spiErrCount;
    csAssert(false);
    s_state = SpiState::Idle;
    s_txBufLen = 0;
    // Leave s_rxReady/rxBufLen alone: the next INTN edge (or a still-set
    // rxReady) will retry.
}

// ---------------------------------------------------------------------------
// sh2 HAL methods
// ---------------------------------------------------------------------------

static int hal_open(sh2_Hal_t* /*self*/)
{
    s_state       = SpiState::Idle;
    s_rxBufLen    = 0;
    s_txBufLen    = 0;
    s_rxReady     = false;

    // Hold the hub in reset while lines settle.
    rstRelease(false);
    csAssert(false);

    // PS0/WAKE (and PS1, tied high on the PCB) must be high from before
    // reset until the first INTN assertion to select SPI mode.
    wakeIdle(true);

    s_inReset = true;   // cleared by HAL_GPIO_EXTI_Callback on first INTN

    HAL_Delay(1);        // >> tnrst (10 ns min)

    rstRelease(true);    // release reset, hub begins booting

    const uint32_t start = HAL_GetTick();
    while (s_inReset) {
        if ((HAL_GetTick() - start) > kResetTimeoutMs) {
            return SH2_ERR_IO;   // no INTN: wiring / power?
        }
    }

    return SH2_OK;
}

static void hal_close(sh2_Hal_t* /*self*/)
{
    criticalEnter();
    (void)HAL_SPI_Abort(s_hspi);
    csAssert(false);
    rstRelease(false);   // hold hub in reset
    s_state    = SpiState::Idle;
    s_rxBufLen = 0;
    s_txBufLen = 0;
    s_rxReady  = false;
    criticalExit();
}

// Non-blocking read: returns a completed packet if one is ready, and frees
// the receive buffer so any write that was waiting for it can proceed.
static int hal_read(sh2_Hal_t* /*self*/, uint8_t* pBuffer, unsigned len,
                    uint32_t* t)
{
    int retval = 0;

    if (s_rxBufLen > 0) {
        if (len >= s_rxBufLen) {
            std::memcpy(pBuffer, s_rxBuf, s_rxBufLen);
            retval = (int)s_rxBufLen;
            *t = s_rxTimestamp_us;
        } else {
            retval = SH2_ERR_BAD_PARAM;   // shouldn't happen: sh2 passes MAX_TRANSFER_IN
        }
        s_rxBufLen = 0;

        criticalEnter();
        spiActivate();
        criticalExit();
    }

    return retval;
}

// Queues a write and requests the hub wake up. Non-blocking: the transfer
// itself happens once INTN asserts (immediately, if the hub is already
// awake and idle).
static int hal_write(sh2_Hal_t* /*self*/, uint8_t* pBuffer, unsigned len)
{
    if ((len > 0) && (pBuffer == nullptr)) {
        return SH2_ERR_BAD_PARAM;
    }
    if (len == 0) {
        return 0;
    }
    if (len > sizeof(s_txBuf)) {
        return SH2_ERR_BAD_PARAM;
    }

    if (s_txBufLen != 0) {
        return 0;   // previous write still pending; sh2 lib retries
    }

    std::memcpy(s_txBuf, pBuffer, len);
    s_txBufLen = len;

    criticalEnter();
    wakeIdle(false);   // assert WAKE
    criticalExit();

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
    if (s_active != nullptr && s_active != this) {
        // sh2 library state is global: one instance only.
        return false;
    }
    if (hspi == nullptr) {
        return false;
    }

    hspi_ = hspi;

    s_active = this;
    s_hspi   = hspi_;

    // Opens the transport (hardware reset inside hal_open) and processes
    // the hub's startup traffic. Blocks internally until done.
    if (sh2_open(&s_hal, asyncCallback, this) != SH2_OK) {
        s_active = nullptr;
        s_hspi   = nullptr;
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
    if (s_active == this) {
        s_active = nullptr;
        s_hspi   = nullptr;
    }
}

void BNO085::service()
{
    if (!open_) {
        return;
    }

    spiWatchdog();
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
