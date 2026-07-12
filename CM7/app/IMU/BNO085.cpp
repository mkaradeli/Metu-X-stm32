/*
 * BNO085.cpp
 *
 *  Created on: Jul 12, 2026
 *      Author: karadeli
 *
 * Implementation. The CEVA sh2_Hal_t transport is implemented here as
 * file-local static functions (polled, blocking I2C), so sh2_hal_init.h /
 * spi_hal.c are not needed at all.
 */

#include "app_main.hpp"         // micros()
#include "BNO085.hpp"

#include <cstring>

extern "C" {
#include "sh2_hal.h"     // sh2_Hal_t, SH2_HAL_MAX_TRANSFER_*

}
//extern uint64_t micros();

// ---------------------------------------------------------------------------
// Polled I2C sh2 HAL (file-local)
// ---------------------------------------------------------------------------

// Blocking-transfer timeout. Worst-case packet (~384 B) at 400 kHz is ~9 ms
// on the wire; leave headroom for clock stretching.
static constexpr uint32_t kI2cTimeoutMs = 100;

// The sh2 library is a singleton (global state in sh2.c/shtp.c), so a single
// active instance is both sufficient and required.
static BNO085* s_active = nullptr;

// Set by BNO085::begin() before sh2_open() so the HAL knows where to talk.
static I2C_HandleTypeDef* s_hi2c = nullptr;
static uint16_t s_addr8 = 0;

static volatile uint32_t s_rxPacketCount = 0;
static volatile uint32_t s_i2cErrCount   = 0;

static uint32_t timeNowUs()
{
    return (uint32_t)micros();
}

static int hal_open(sh2_Hal_t* /*self*/)
{
    // SHTP soft-reset packet: len=5 (LSB,MSB), channel 1 (executable),
    // seq 0, command 1 = reset.
    static const uint8_t softreset_pkt[] = {5, 0, 1, 0, 1};

    // The hub NACKs while booting; retry a few times.
    bool ok = false;
    for (int attempt = 0; attempt < 5; ++attempt) {
        if (HAL_I2C_Master_Transmit(s_hi2c, s_addr8,
                                    const_cast<uint8_t*>(softreset_pkt),
                                    sizeof(softreset_pkt),
                                    kI2cTimeoutMs) == HAL_OK) {
            ok = true;
            break;
        }
        HAL_Delay(30);
    }
    if (!ok) {
        return SH2_ERR_IO;   // no ACK: wiring / address (0x4A vs 0x4B)?
    }

    // Let the hub reboot. The advertisement + reset-complete packets will
    // then be pending; sh2_open() collects them through hal_read().
    HAL_Delay(300);

    return SH2_OK;
}

static void hal_close(sh2_Hal_t* /*self*/)
{
    // Nothing to tear down: peripheral is owned by CubeMX code.
}

// Poll for one SHTP packet. Returns 0 when nothing is pending (the normal
// case between reports).
static int hal_read(sh2_Hal_t* /*self*/, uint8_t* pBuffer, unsigned len,
                    uint32_t* t)
{
    uint8_t header[4];

    // 1) Read the 4-byte SHTP header to learn the cargo length.
    if (HAL_I2C_Master_Receive(s_hi2c, s_addr8, header, sizeof(header),
                               kI2cTimeoutMs) != HAL_OK) {
        ++s_i2cErrCount;
        return 0;
    }

    uint16_t packetSize =
        (uint16_t)(header[0] | (header[1] << 8)) & (uint16_t)~0x8000;

    // 0x0000: nothing pending. 0x7FFF (0xFFFF masked): bus garbage /
    // device not ready.
    if (packetSize == 0 || packetSize == 0x7FFF) {
        return 0;
    }

    // Clamp to the caller's buffer; the device re-sends the remainder as a
    // continuation transfer, which the shtp layer reassembles.
    if (packetSize > len) {
        packetSize = (uint16_t)len;
    }

    // Timestamp as close to the data as we can get without INTN.
    *t = timeNowUs();

    // 2) Read the whole packet. The device re-sends the header
    //    (continuation bit set) followed by the payload; shtp masks the
    //    bit itself, so hand over the buffer as-is.
    if (HAL_I2C_Master_Receive(s_hi2c, s_addr8, pBuffer, packetSize,
                               kI2cTimeoutMs) != HAL_OK) {
        ++s_i2cErrCount;
        return 0;
    }

    ++s_rxPacketCount;
    return (int)packetSize;
}

static int hal_write(sh2_Hal_t* /*self*/, uint8_t* pBuffer, unsigned len)
{
    if ((len > 0) && (pBuffer == nullptr)) {
        return SH2_ERR_BAD_PARAM;
    }
    if (len == 0) {
        return 0;
    }

    if (HAL_I2C_Master_Transmit(s_hi2c, s_addr8, pBuffer, (uint16_t)len,
                                kI2cTimeoutMs) != HAL_OK) {
        ++s_i2cErrCount;
        return 0;   // sh2 lib retries
    }
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

bool BNO085::begin(I2C_HandleTypeDef* hi2c, uint8_t addr7)
{
    if (s_active != nullptr && s_active != this) {
        // sh2 library state is global: one instance only.
        return false;
    }
    if (hi2c == nullptr) {
        return false;
    }

    hi2c_  = hi2c;
    addr8_ = (uint16_t)(addr7 << 1);

    s_active = this;
    s_hi2c   = hi2c_;
    s_addr8  = addr8_;

    // Quick presence check for a clearer failure mode.
    if (HAL_I2C_IsDeviceReady(hi2c_, addr8_, 3, kI2cTimeoutMs) != HAL_OK) {
        // Not fatal by itself -- the hub may be mid-boot and NACKing --
        // hal_open() retries. But if that also fails, sh2_open reports it.
    }

    // Opens the transport (soft reset inside hal_open) and processes the
    // hub's startup traffic. Blocks internally until done.
    if (sh2_open(&s_hal, asyncCallback, this) != SH2_OK) {
        s_active = nullptr;
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
    }
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
        // Re-arm the flag for the user-visible wasReset() as a one-shot:
        // note: reapply already happened, this is informational.
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
uint32_t BNO085::i2cErrorCount() const { return s_i2cErrCount; }

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

    switch (value.sensorId) {
    case SH2_LINEAR_ACCELERATION:
        linearAccel = value.un.linearAcceleration;
        accelInterval_us = now - lastAccelStamp_us_;
        lastAccelStamp_us_ = now;
        newAccel_ = true;
        break;

    case SH2_GAME_ROTATION_VECTOR:
        quaternion = value.un.gameRotationVector;
        quaternionInterval_us = now - lastQuatStamp_us_;
        lastQuatStamp_us_ = now;
        newQuaternion_ = true;
        break;

    default:
        lastValue = value;
        newValue_ = true;
        break;
    }
}
