/*
 * AltitudeEstimator.hpp
 *
 *  Created on: Aug 17, 2026
 *      Author: karadeli
 */

#ifndef ALTITUDEESTIMATOR_HPP_
#define ALTITUDEESTIMATOR_HPP_
/*
 * AltitudeEstimator.hpp
 *
 * 4-state Kalman filter fusing a body-fixed downward TF02-Pro lidar, a
 * BMP581 barometer, and a BNO085 IMU to estimate vertical position and
 * velocity of a VTVL rocket.
 *
 *   x = [ h  v  ba  bb ]'
 *       h  : height of the reference point above the pad plane      [m]
 *       v  : vertical velocity, up positive                         [m/s]
 *       ba : residual vertical accel bias, world frame               [m/s^2]
 *       bb : barometer height bias (raw pressure-height minus truth) [m]
 *
 * Lidar corrects h directly (H = [1 0 0 0]) and carries three gates:
 *   - tilt (cosTiltMin): hard geometry limit, the reading is physically
 *     meaningless past this angle, not a judgment call.
 *   - obstruction (freefallEnable): the vehicle's own one-directional
 *     (upward) thrust means it can never lose altitude faster than gravity
 *     on its own, so a reading that implies exactly that -- an
 *     instantaneous drop no ballistic trajectory can match -- means
 *     something crossed the beam (a ground umbilical, a cable), not that
 *     the vehicle actually fell that fast. Only holds under that
 *     assumption: a hand-shake bench test CAN push the sensor down faster
 *     than gravity, so disable this for that kind of ground testing.
 *   - nisGate: a loose plausibility check on top of both of the above,
 *     meant to catch genuinely implausible single readings (specular
 *     return, sensor glitch), not to second-guess real dynamics -- kept
 *     deliberately loose after a tight (~3-sigma) version previously
 *     rejected ordinary fast motion whenever the filter's own uncertainty
 *     model was even slightly too tight, and a diverged filter would then
 *     keep rejecting the very corrections that would fix it (visible as a
 *     sawtooth in the fused output).
 *
 * Baro corrects h *and* its own bias jointly (H = [1 0 0 1], since the raw
 * pressure-height measurement is h + bb by construction): far noisier but
 * essentially never catastrophically wrong (no beam to obstruct, no
 * specular returns), so it carries no rejection gate at all -- its own
 * (larger) measurement noise is the only thing limiting its influence. bb
 * is a slow random walk (sigmaBaroBiasWalk); lidar corrections refine it
 * automatically through the h/bb covariance term, same Kalman math as
 * everything else here, not a hand-rolled re-anchor rule.
 *
 * Both correction paths route through the same general (arbitrary H)
 * update -- see applyCorrection()/quadForm() -- so their only real
 * difference is which H they pass and whether they gate before calling it.
 *
 * The whole state is self-healing: isStateFinite()/recoverFromNaN() catch
 * a NaN/Inf state (nothing else here can ever clear one on its own -- NaN
 * minus anything is NaN, forever) and force-reset it rather than staying
 * permanently corrupted.
 *
 * Quaternion convention: q[4] = { w, x, y, z }, rotates BODY -> WORLD.
 * World Z is up and gravity aligned (BNO085 rotation vector frame).
 * Body frame as mounted: X+ right, Y+ front, Z+ up.
 *
 * No heap, no exceptions, no RTTI, no STL. Single precision throughout
 * except the calibration accumulators. Not reentrant: call from one
 * context, or guard with a critical section.
 */

#include <cstdint>

class AltitudeEstimator
{
public:
    /* ---- lidar frame gating / decimation ------------------------------ */
    static constexpr uint8_t  kLidarBlock    = 5;      /* 1 kHz -> 200 Hz  */
    static constexpr uint8_t  kLidarMinValid = 3;      /* else drop block  */
    static constexpr uint16_t kStrengthMin   = 100;    /* sensor flags <60 */
    static constexpr uint16_t kStrengthSat   = 65535;  /* specular return  */
    static constexpr uint16_t kDistMaxMm     = 44000;  /* 4500 cm "no ret" */
    static constexpr uint16_t kDistMinMm     = 40;     /* below spec, see README */

    /* ---- calibration sample minimums ---------------------------------- */
    static constexpr uint32_t kCalMinImu   = 100;
    static constexpr uint32_t kCalMinLidar = 40;
    static constexpr uint32_t kCalMinBaro  = 40;   /* optional: baro fusion
                                                       just stays off if the
                                                       pad window didn't see
                                                       enough baro samples */

    enum class Phase : uint8_t { Idle, Calibrating, Running };

    struct Params
    {
        /* process */
        float sigmaAccel  = 0.30f;    /* vertical accel noise 1s [m/s^2]    */
        float sigmaBias   = 0.02f;    /* accel bias random walk [m/s^2/sqrt s] */
        float sigmaBaroBiasWalk = 0.002f; /* baro bias random walk [m/sqrt s]
                                        * Slow on purpose -- thermal/pressure
                                        * drift over the whole flight, not
                                        * sample-to-sample noise (sigmaBaro
                                        * below is that). Untuned guess,
                                        * re-check against real flight data. */

        /* measurement */
        float sigmaRange0 = 0.012f;   /* lidar noise floor, post-avg  [m]   */
        float sigmaRangeK = 0.004f;   /* range-proportional term      [-]   */
        float sigmaTilt   = 0.0175f;  /* attitude 1s                  [rad] */
        float sigmaBaro   = 0.3f;    /* baro height noise, 1-sigma   [m]
                                        * Measured (bench), not the sensor's
                                        * datasheet noise -- real dynamic
                                        * pressure in flight (ram air, prop
                                        * wash) can still push the actual
                                        * error above this, so re-check
                                        * against real flight telemetry once
                                        * available and prefer under-trusting
                                        * it over over-trusting it. */

        /* gating -- see class comment above for what each one is for and
         * why nisGate is loose. */
        float cosTiltMin  = 0.819f;   /* reject lidar past 35 deg           */
        float nisGate     = 25.0f;    /* ~5s chi-square on 1-D innovation   */
        uint32_t rejectsBeforeInflate = 40;

        /* obstruction gate: the vehicle cannot lose altitude faster than
         * gravity on its own (one-directional thrust), so anything below
         * that floor is a foreign object crossing the beam (cable, ground
         * umbilical) rather than the vehicle. Only valid under that
         * assumption -- a hand-shake bench test can violate it, disable
         * for that kind of ground testing. */
        bool  freefallEnable = true;
        float freefallG      = 9.81f;  /* downward accel bound   [m/s^2]    */
        float freefallMargin = 0.05f;  /* fixed slack            [m]        */
        float freefallSigmaK = 3.0f;   /* sigma multiplier on the margin    */
        float freefallMaxTau = 2.0f;   /* stale anchor -> disarm  [s]       */

        /* geometry, from your mount survey */
        float beam[3]  = { 0.0f, 0.0f, -1.0f }; /* beam unit vec, body frame */
        float lever[3] = { 0.0f, 0.0f,  0.0f }; /* p_face - p_ref, body [m]  */
    };

    struct Status
    {
        float    innovation          = 0.0f;  /* last y = z - h        [m] */
        float    nis                 = 0.0f;  /* last y^2 / S          [-] */
        float    cosTilt             = 1.0f;  /* vertical fraction     [-] */
        uint32_t lidarAccepted       = 0;
        uint32_t lidarRejected       = 0;     /* gate or tilt failures     */
        uint32_t lidarImplausible    = 0;     /* obstruction-gate rejections */
        uint32_t lidarBlocksDropped  = 0;     /* too few valid raw frames  */
        uint32_t consecutiveRejects  = 0;
        bool     lastUpdateAccepted  = false;
        uint32_t nanRecoveries       = 0;     /* state was non-finite and
                                                  got force-reset -- should
                                                  stay 0; if it's climbing,
                                                  something is still feeding
                                                  bad values in */

        float    baroInnovation      = 0.0f;  /* last y = z - (h+bb)   [m] */
        float    baroNis             = 0.0f;  /* tracked, not gated        */
        uint32_t baroAccepted        = 0;
        bool     lastBaroUpdateAccepted = false;

        /* Which height source(s) actually had enough samples when
         * finishCalibration() ran -- check these after calibration so a
         * missing/dead lidar (or baro) at boot is a loud, deliberate fact
         * instead of a silent "stuck in Calibrating forever". */
        bool     lidarCalibratedAtStart = false;
        bool     baroCalibratedAtStart  = false;
    };

    AltitudeEstimator() { reset(); }

    void configure(const Params& p) { params_ = p; }
    const Params& params() const    { return params_; }
    void reset();

    /* ---- pad calibration ---------------------------------------------
     * Hold the vehicle stationary, call beginCalibration(), keep feeding
     * all sensors for ~1.5 s, then finishCalibration(). This sets the
     * gravity reference (folding in accel scale error), the initial
     * height, the initial baro bias, and zeroes the accel bias state.
     *
     * Only IMU is mandatory for finishCalibration() to succeed -- lidar and
     * baro are each independently optional. A missing/dead one just means
     * its own fusion stays disabled (pushLidarFrame()/pushBaroFrame()
     * become no-ops for height correction) and the initial height/
     * uncertainty falls back to whichever of the other did calibrate, or a
     * wide-open guess if neither did. Check status().lidarCalibratedAtStart
     * / baroCalibratedAtStart after a successful call to know which
     * reference you actually got. */
    void beginCalibration();
    bool finishCalibration();

    /* ---- run time -----------------------------------------------------
     * All three entry points are phase aware: during Calibrating they
     * accumulate, during Running they propagate / correct. */

    /* Call at IMU rate. Use SH-2 ACCELEROMETER (gravity included), not
     * LINEAR_ACCELERATION. Prefer the report timestamp delta for dt. */
    void pushImu(const float aBody[3], const float q[4], float dt);

    /* Call once per raw TF02-Pro frame at 1 kHz. Handles validity gating
     * and 5-sample decimation internally; performs a filter update only on
     * a completed valid block. Returns true if the filter was corrected. */
    bool pushLidarFrame(uint16_t distMm, uint16_t strength, const float q[4]);

    /* Call once per BMP581 reading (raw pressure, Pa). Phase-aware like
     * pushLidarFrame(): accumulates the pad baseline during Calibrating,
     * applies an ungated joint (h, baroBias) correction during Running.
     * No-op if baro wasn't part of the calibration window (see
     * beginCalibration()). Returns true if the filter was corrected. */
    bool pushBaroFrame(float pressurePa);

    /* Retune in flight, e.g. from your mission phase machine. */
    void setProcessNoise(float sigmaAccel, float sigmaTilt);

    /* Open the covariance back up after an inertial coast. */
    void inflate(float kPos, float kVel);

    /* ---- accessors ---------------------------------------------------- */
    float height()   const { return x_[0]; }
    float velocity() const { return x_[1]; }
    float bias()     const { return x_[2]; }
    float baroBias() const { return x_[3]; }
    float heightSigma()   const;
    float velocitySigma() const;
    float gravityRef() const { return gStatic_; }
    Phase phase()      const { return phase_; }
    const Status& status() const { return status_; }

    /* Tilt-projected lidar height (cosTilt * range - lever_z) from the most
     * recent valid pushLidarFrame() block -- the same geometry update()
     * feeds into the Kalman correction, exposed here BEFORE any of the
     * filter's own gating (tilt aside) or correction is applied. Use this
     * if you want lidar's raw tilt-corrected reading directly, decoupled
     * from the filter's fusion/gating decisions. */
    float lastProjectedHeight()      const { return lastProjectedZ_; }
    bool  lastProjectedHeightValid() const { return lastProjectedZValid_; }

    /* Clear-on-read new-value flag for lastProjectedHeight(), mirroring
     * Lidar::hasNewReading()/Barometer::hasNewReading(). */
    bool hasNewProjectedHeight() {
        bool r = lastProjectedZNew_;
        lastProjectedZNew_ = false;
        return r;
    }

private:
    static void  quatToR(const float q[4], float R[3][3]);
    static float bodyToWorldZ(const float q[4], const float v[3]);
    static void  mat4Mul(const float A[4][4], const float B[4][4], float C[4][4]);
    static void  mat4MulBt(const float A[4][4], const float B[4][4], float C[4][4]);
    static float quadForm(const float P[4][4], const float H[4]); /* H P H^T */

    /* Barometric formula against a FIXED reference (not local QNH) -- the
     * absolute reference is absorbed into the bb state, so it doesn't need
     * to be accurate, only stable. */
    static float pressureToHeight(float pressurePa);

    void symmetrise();
    bool projectLidar(float range, const float q[4],
                      float& z, float& cosTilt) const;
    void predict(float u, float dt);
    bool update(float range, const float q[4]);

    /* Shared general (arbitrary H) correction math for both lidar
     * (H=[1 0 0 0]) and baro (H=[1 0 0 1]). innovationStats() is pure (no
     * state touched) so a caller can gate on nis before deciding whether to
     * call applyCorrection() at all -- lidar needs that, baro doesn't. */
    static void innovationStats(float HPHt, float Rm, float y, float& S, float& nis);
    void applyCorrection(const float H[4], float y, float Rm);

    /* Self-healing against a NaN/Inf state -- see class comment. */
    bool isStateFinite() const;
    void recoverFromNaN();

    Params  params_;
    Status  status_;
    Phase   phase_;

    float   x_[4];
    float   P_[4][4];
    float   gStatic_;

    /* obstruction-gate anchor: last accepted lidar state and its age */
    float    anchorH_;
    float    anchorV_;
    float    anchorTau_;
    bool     anchorValid_;

    /* see lastProjectedHeight() */
    float    lastProjectedZ_;
    bool     lastProjectedZValid_;
    bool     lastProjectedZNew_;

    /* decimator */
    uint32_t blockSum_;
    uint8_t  blockValid_;
    uint8_t  blockSeen_;

    /* calibration accumulators */
    double   calSumAz_;
    double   calSumH_;
    double   calSumBaro_;
    uint32_t calNAz_;
    uint32_t calNH_;
    uint32_t calNBaro_;

    bool     baroCalibrated_;
};
#endif /* ALTITUDEESTIMATOR_HPP_ */
