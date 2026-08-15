/**
 * AltitudeKf.hpp - Tilt-compensated altitude / vertical-velocity Kalman filter
 *
 * Freestanding C++17. No heap, no exceptions, no RTTI, no virtuals.
 * Only <cmath>'s sqrtf is required from the runtime.
 *
 * States: [ h  v  b ]
 *   h : altitude of the body origin above the ground plane      [m]   (up +)
 *   v : vertical velocity in world frame                        [m/s] (up +)
 *   b : accelerometer bias along the BODY z axis                [m/s^2]
 *
 * Frame convention:
 *   world = ENU (Z up), matching the BNO085 SH-2 reference frame
 *   body +z along the rocket's longitudinal axis, nose-up
 *   lidar beam vector points at the ground, nominally {0, 0, -1}
 *
 * Only the third row of R_wb is ever needed, so the entire attitude coupling
 * costs three dot products per predict.
 *
 * Not internally synchronised. Call predict() and updateLidar() from the same
 * context, or guard the instance yourself.
 */

#ifndef GNC_ALTITUDE_KF_HPP
#define GNC_ALTITUDE_KF_HPP

#include <cstdint>
#include <cstddef>

namespace gnc {

// ---------------------------------------------------------------------------
// small value types
// ---------------------------------------------------------------------------

struct Vec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};

    constexpr float dot(const Vec3& o) const noexcept
    {
        return x * o.x + y * o.y + z * o.z;
    }
};

/** Body -> world unit quaternion, scalar first. */
struct Quat {
    float w{1.0f}, x{0.0f}, y{0.0f}, z{0.0f};

    /** Third row of the body->world DCM, i.e. e_up^T * R_wb.
     *  Projects any body-frame vector onto world Up. */
    constexpr Vec3 upRow() const noexcept
    {
        return Vec3{ 2.0f * (x * z - w * y),
                     2.0f * (y * z + w * x),
                     1.0f - 2.0f * (x * x + y * y) };
    }
};

// ---------------------------------------------------------------------------
// median prefilter - use the surplus lidar rate for spike rejection, not for
// update rate. N must be odd. Cheap insertion sort; keep N <= 9.
// ---------------------------------------------------------------------------

template <std::size_t N>
class MedianFilter {
    static_assert(N % 2u == 1u, "MedianFilter window must be odd");
    static_assert(N >= 3u && N <= 15u, "keep the window small");

public:
    void reset() noexcept { count_ = 0; idx_ = 0; }

    /** Push a sample. Returns true and writes @p out once the window is full. */
    bool push(float v, float& out) noexcept
    {
        buf_[idx_] = v;
        idx_ = (idx_ + 1u) % N;
        if (count_ < N) { ++count_; }
        if (count_ < N) { return false; }

        float s[N];
        for (std::size_t i = 0; i < N; ++i) { s[i] = buf_[i]; }
        for (std::size_t i = 1; i < N; ++i) {          // insertion sort
            const float key = s[i];
            std::size_t j = i;
            while (j > 0 && s[j - 1] > key) { s[j] = s[j - 1]; --j; }
            s[j] = key;
        }
        out = s[N / 2u];
        return true;
    }

    bool ready() const noexcept { return count_ >= N; }

private:
    float       buf_[N]{};
    std::size_t idx_{0};
    std::size_t count_{0};
};

// ---------------------------------------------------------------------------
// the filter
// ---------------------------------------------------------------------------

class AltitudeKf {
public:
    struct Config {
        // ---- process noise ----
        /// Accelerometer white-noise PSD [(m/s^2)^2 / Hz].
        /// Set this from a VIBRATION measurement on the vehicle, not from the
        /// BNO085 datasheet noise density.
        float qAccel{0.30f * 0.30f};
        /// Accel bias random-walk PSD [(m/s^2)^2 / s].
        float qBias{0.005f * 0.005f};

        // ---- measurement noise ----
        /// Irreducible range noise floor [m].
        float sigmaRangeFloor{0.025f};
        /// Fractional range noise [-]. Only used by the no-strength overload.
        float sigmaRangeFrac{0.010f};
        /// Strength model: var_r = floor^2 + kStrength / strength.
        /// Fit against static data; see calibration note in the .cpp.
        float kStrength{50.0f};
        /// 1-sigma tilt (roll/pitch) error of the attitude source [rad].
        float sigmaTilt{2.0f * 0.017453293f};

        // ---- gating ----
        float    nisGate{9.0f};      ///< chi2(1), 3-sigma
        float    minTiltCos{0.866f}; ///< reject beyond 30 deg
        float    minRange{0.20f};    ///< [m]
        float    maxRange{25.0f};    ///< [m]
        uint16_t minStrength{100u};  ///< Benewake weak-return threshold
        float    maxDt{0.25f};       ///< reject absurd timesteps [s]

        // ---- geometry, body frame ----
        Vec3 lidarPos{0.0f, 0.0f, -0.35f};  ///< aperture w.r.t. body origin [m]
        Vec3 lidarBeam{0.0f, 0.0f, -1.0f};  ///< unit vector toward the ground
    };

    struct State {
        float altitude{0.0f};   ///< [m]
        float velocity{0.0f};   ///< [m/s]
        float accelBias{0.0f};  ///< [m/s^2], body z
    };

    enum class UpdateResult : uint8_t {
        Accepted = 0,
        NotInitialised,
        RangeOutOfBounds,
        WeakSignal,
        TiltTooLarge,
        NisGated,
        Singular
    };

    // -- construction ------------------------------------------------------

    AltitudeKf() noexcept = default;
    explicit AltitudeKf(const Config& cfg) noexcept : cfg_(cfg) {}

    Config&       config() noexcept       { return cfg_; }
    const Config& config() const noexcept { return cfg_; }

    /**
     * Initialise on the pad.
     * @param h0     altitude of the body origin above ground [m].
     *               Typically  h0 = c * mean(range) - lambda  from a static average.
     * @param b0     initial body-z accel bias estimate [m/s^2].
     * @param sigmaH 1-sigma initial altitude uncertainty [m].
     * @param sigmaV 1-sigma initial velocity uncertainty [m/s].
     * @param sigmaB 1-sigma initial bias uncertainty [m/s^2].
     */
    void init(float h0, float b0,
              float sigmaH, float sigmaV, float sigmaB) noexcept;

    void reset() noexcept;

    // -- time update -------------------------------------------------------

    /**
     * @param dt            elapsed time [s]
     * @param specificForce calibrated accel in body frame [m/s^2].
     *                      Reads +g upward when static. Use SH2_ACCELEROMETER,
     *                      not SH2_LINEAR_ACCELERATION.
     * @param q             body->world quaternion (game rotation vector)
     * @return false if dt was rejected as out of range
     */
    bool predict(float dt, const Vec3& specificForce, const Quat& q) noexcept;

    /**
     * Timestamp-driven overload. Derives dt from a free-running cycle counter
     * (DWT->CYCCNT), so a wrong nominal loop rate cannot silently corrupt the
     * integration. Unsigned arithmetic handles wraparound.
     *
     * Call setCycleRate() first. The first call after init/reset only seeds the
     * timestamp and returns false.
     */
    bool predictAt(uint32_t cycleCount, const Vec3& specificForce,
                   const Quat& q) noexcept;

    /**
     * Microsecond-timestamp overload. Preferred when a 64-bit us counter is
     * available: it never wraps, so no rate configuration is needed and no
     * wraparound reasoning is required.
     *
     * The first call after init()/reset() only seeds the timestamp.
     */
    bool predictAtMicros(uint64_t nowUs, const Vec3& specificForce,
                         const Quat& q) noexcept;

    /// e.g. setCycleRate(480.0e6f) for a 480 MHz Cortex-M7 core clock.
    void setCycleRate(float hz) noexcept { cycleRate_ = hz; haveStamp_ = false; }

    // -- measurement update ------------------------------------------------

    /** Slant-range update using the strength-derived noise model (preferred). */
    UpdateResult updateLidar(float range, uint16_t strength) noexcept;

    /** Slant-range update using the fractional noise model (no strength). */
    UpdateResult updateLidar(float range) noexcept;

    /** Zero-velocity pseudo-measurement. Use while clamped or on the legs. */
    void updateZeroVelocity(float sigmaV) noexcept;

    // -- accessors ---------------------------------------------------------

    State state() const noexcept { return State{x_[0], x_[1], x_[2]}; }

    float altitude()  const noexcept { return x_[0]; }
    float velocity()  const noexcept { return x_[1]; }
    float accelBias() const noexcept { return x_[2]; }

    float altitudeSigma() const noexcept { return safeSqrt(P_[0][0]); }
    float velocitySigma() const noexcept { return safeSqrt(P_[1][1]); }
    float biasSigma()     const noexcept { return safeSqrt(P_[2][2]); }

    /// Covariance, row-major. Exposed for telemetry and offline analysis.
    const float (&covariance() const noexcept)[3][3] { return P_; }

    bool  initialised()   const noexcept { return initialised_; }
    /// cos of the beam-to-vertical angle cached at the last predict.
    float tiltCosine()    const noexcept { return c_; }
    float lastInnovation() const noexcept { return lastNu_; }
    float lastNis()        const noexcept { return lastNis_; }
    uint32_t rejectCount() const noexcept { return rejects_; }
    UpdateResult lastResult() const noexcept { return lastResult_; }

private:
    UpdateResult applyRange(float range, float varRange) noexcept;
    void forceSymmetry() noexcept;
    static float safeSqrt(float v) noexcept;

    Config cfg_{};

    float x_[3]{0.0f, 0.0f, 0.0f};
    float P_[3][3]{};

    // cached at the last predict
    Vec3  upRow_{0.0f, 0.0f, 1.0f};
    float c_{1.0f};     ///< beam tilt cosine  = -(upRow . lidarBeam)
    float cz_{1.0f};    ///< body-z tilt cosine = upRow.z
    float lambda_{0.0f};///< vertical lever arm = upRow . lidarPos  [m]

    // timestamp bookkeeping
    float    cycleRate_{480.0e6f};
    uint32_t lastStamp_{0};
    uint64_t lastStampUs_{0};
    bool     haveStamp_{false};
    bool     haveStampUs_{false};

    // diagnostics
    float        lastNu_{0.0f};
    float        lastNis_{0.0f};
    uint32_t     rejects_{0};
    UpdateResult lastResult_{UpdateResult::NotInitialised};
    bool         initialised_{false};
};

}  // namespace gnc

#endif  // GNC_ALTITUDE_KF_HPP
