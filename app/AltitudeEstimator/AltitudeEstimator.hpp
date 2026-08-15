/*
 * AltitudeEstimator.hpp
 *
 * Integration wrapper around gnc::AltitudeKf for the METU-X flight computer.
 *
 * Owns the filter, the median prefilter, and a lock-free queue that carries
 * lidar frames from the UART6 idle ISR into the tim7 context, so that the
 * filter state is only ever touched from ONE interrupt priority level.
 *
 * Threading contract:
 *   onLidarFrame()  - called from HAL_UARTEx_RxEventCallback (UART6 ISR)   1 kHz
 *   service()       - called from tim7_trigger() after imu.service()       1 kHz
 *   accessors       - safe from anywhere; they read plain floats and may
 *                     tear across a service() call. For logging that is fine.
 *
 * NEVER call service() from app_loop(): app_loop can block for tens of
 * milliseconds inside FatFs, which would stall the prediction step and
 * silently corrupt the integration.
 */

#ifndef GNC_ALTITUDE_ESTIMATOR_HPP
#define GNC_ALTITUDE_ESTIMATOR_HPP

#include "AltitudeKf.hpp"

#include <atomic>
#include <cstdint>

namespace gnc {

class AltitudeEstimator {
public:
    enum class Phase : uint8_t {
        Idle = 0,       ///< begin() not yet called
        Calibrating,    ///< averaging accelerometer bias on the pad
        Running         ///< filter live
    };

    /**
     * Result of the preflight lidar cross-check.
     *
     * h0 is NOT derived from the lidar: the standing height is a mechanical
     * constant, known from the drawing to better precision than the sensor can
     * measure it. The lidar's job on the pad is to AGREE with that constant.
     * Disagreement means a wrong lever arm, wrong units, an obstruction, or a
     * tilted pad -- all things you want to find before launch, not after.
     */
    enum class PadCheck : uint8_t {
        Pending = 0,    ///< calibration not finished
        Ok,             ///< lidar agrees with the mechanical standing height
        NoReturns,      ///< too few valid frames to check
        Mismatch        ///< lidar disagrees by more than padCheckTol
    };

    // ---- geometry: SET THESE FROM YOUR MECHANICAL DRAWING -----------------
    // lidarPos is the aperture position relative to the body origin used by
    // your control laws. lidarBeam is the beam direction in body frame.
    // Verify the quaternion convention with the r3.f_body bench test before
    // trusting any of this. See README section 3.
    // -----------------------------------------------------------------------

    /** Arm pad calibration. Call at the end of app_init(). */
    void begin() noexcept;

    /** Abandon the current estimate and re-run pad calibration. */
    void restart() noexcept;

    /**
     * Lidar frame handler. Call from the UART6 idle-line callback, after
     * lidar.FrameHandler().
     *
     * @param distanceMm raw TF02-Pro distance, millimetres
     * @param strength   raw TF02-Pro signal strength
     *
     * Runs a median-of-5 and decimates 1 kHz -> 200 Hz. Does no filter maths;
     * it only enqueues. Costs ~40 cycles.
     */
    void onLidarFrame(uint16_t distanceMm, uint16_t strength) noexcept;

    /**
     * Filter service. Call from tim7_trigger() immediately after imu.service().
     *
     * @param newImu   true when a fresh quaternion+accel pair is available
     * @param q        body->world quaternion from SH2_GAME_ROTATION_VECTOR
     * @param accel    body-frame specific force from SH2_ACCELEROMETER
     *                 (NOT SH2_LINEAR_ACCELERATION - see README 5.2)
     * @param nowUs    micros()
     */
    void service(bool newImu, const Quat& q, const Vec3& accel,
                 uint64_t nowUs) noexcept;

    /**
     * Altitude of the body origin when the lander is down, in metres.
     *
     * With the body origin at the landing contact plane this is 0.0 by
     * construction, which is the default. Set it if your origin is elsewhere.
     */
    void setPadHeight(float h) noexcept { padHeightM_ = h; }

    /** Tolerance for the preflight lidar cross-check [m]. Default 0.05. */
    void setPadCheckTol(float t) noexcept { padCheckTolM_ = t; }

    /** While true, a zero-velocity update is applied each service tick. */
    void setClamped(bool clamped) noexcept { clamped_ = clamped; }

    // ---- accessors (safe to call from any context) ------------------------
    Phase phase()      const noexcept { return phase_; }
    bool  valid()      const noexcept { return phase_ == Phase::Running; }
    float altitude()   const noexcept { return kf_.altitude(); }
    float velocity()   const noexcept { return kf_.velocity(); }
    float accelBias()  const noexcept { return kf_.accelBias(); }
    float altitudeSigma() const noexcept { return kf_.altitudeSigma(); }
    float velocitySigma() const noexcept { return kf_.velocitySigma(); }
    float lastNis()    const noexcept { return kf_.lastNis(); }

    /**
     * Microseconds since the last ACCEPTED lidar update. The landing controller
     * should watch this: the last few centimetres of descent are expected to
     * lose the lidar to its blind zone, and that is normal, not a fault.
     * Returns UINT64_MAX before the first accepted update.
     */
    uint64_t lidarStaleUs(uint64_t nowUs) const noexcept {
        return (lastAcceptUs_ == 0u) ? UINT64_MAX : (nowUs - lastAcceptUs_);
    }
    float meanNis()    const noexcept { return meanNis_; }

    PadCheck padCheck() const noexcept { return padCheck_; }
    /// Lidar-minus-mechanical standing height discrepancy [m]. Log this.
    float    padCheckError() const noexcept { return padCheckErr_; }
    /// Gate arming on this: calibration finished AND the lidar agreed.
    bool     readyToArm() const noexcept {
        return phase_ == Phase::Running && padCheck_ == PadCheck::Ok;
    }
    float tiltCosine() const noexcept { return kf_.tiltCosine(); }
    uint32_t rejectCount()   const noexcept { return kf_.rejectCount(); }
    uint32_t queueOverflow() const noexcept { return qOverflow_; }
    uint8_t  lastResult()    const noexcept {
        return static_cast<uint8_t>(kf_.lastResult());
    }

    AltitudeKf&       filter() noexcept       { return kf_; }
    const AltitudeKf& filter() const noexcept { return kf_; }

private:
    struct LidarSample {
        float    rangeM;
        uint16_t strength;
    };

    static constexpr uint32_t kQueueLen  = 8u;      // power of two
    static constexpr uint32_t kQueueMask = kQueueLen - 1u;
    static constexpr uint32_t kCalSamples = 800u;   // ~2 s at 400 Hz
    /// TF02-Pro reports MILLIMETRES on this build. Set to 1.0f if your Lidar
    /// class already converts to metres.
    static constexpr float    kRawToM = 0.001f;

    bool popLidar(LidarSample& out) noexcept;
    void runCalibration(const Quat& q, const Vec3& accel) noexcept;

    AltitudeKf       kf_{};
    MedianFilter<5>  med_{};

    // SPSC queue: producer = UART6 ISR, consumer = tim7
    LidarSample           queue_[kQueueLen]{};
    std::atomic<uint32_t> qHead_{0};
    std::atomic<uint32_t> qTail_{0};
    uint32_t              qOverflow_{0};
    uint8_t               decim_{0};

    // pad calibration accumulators (running means, no precision loss)
    uint32_t calN_{0};
    uint32_t calRangeN_{0};
    float    calUpAccel_{0.0f};
    float    calRange_{0.0f};
    float    calTiltCos_{1.0f};
    float    calLambda_{0.0f};

    float    meanNis_{0.0f};
    uint64_t lastAcceptUs_{0};
    float    padHeightM_{0.0f};
    float    padCheckTolM_{0.05f};
    float    padCheckErr_{0.0f};
    PadCheck padCheck_{PadCheck::Pending};
    Phase  phase_{Phase::Idle};
    bool   clamped_{true};
};

}  // namespace gnc

/// The one instance. Defined in globals.cpp.
extern gnc::AltitudeEstimator altEstimator;

#endif  // GNC_ALTITUDE_ESTIMATOR_HPP
