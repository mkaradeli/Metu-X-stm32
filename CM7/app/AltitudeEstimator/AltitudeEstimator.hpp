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
 * 3-state Kalman filter fusing a body-fixed downward TF02-Pro lidar, a
 * BMP581 barometer, and a BNO085 IMU to estimate vertical position and
 * velocity of a VTVL rocket.
 *
 *   x = [ h  v  b ]'
 *       h : height of the reference point above the pad plane      [m]
 *       v : vertical velocity, up positive                         [m/s]
 *       b : residual vertical accel bias, world frame              [m/s^2]
 *
 * Lidar and baro are two independent scalar corrections of the same h
 * state (H = [1 0 0] for both) with very different failure characters:
 * lidar is precise but occasionally *wrong* -- obstructions, specular
 * returns, or a wedged sensor that keeps reporting one stale value forever
 * even as strength stays in-range -- so it carries heavy gating (tilt,
 * NIS, free-fall plausibility, and a stuck-value detector). Baro is far
 * noisier but essentially never catastrophically wrong, so it carries no
 * rejection gate at all: its own (larger) measurement noise is the only
 * thing limiting its influence, and the Kalman gain naturally lets it fall
 * back to near-zero influence whenever lidar has been keeping P[0][0]
 * tight, and take over automatically the moment lidar goes quiet or gets
 * rejected and P[0][0] grows. No explicit source-switching logic needed.
 *
 * Quaternion convention: q[4] = { w, x, y, z }, rotates BODY -> WORLD.
 * World Z is up and gravity aligned (BNO085 rotation vector frame).
 * Body frame as mounted: X+ right, Y+ front, Z+ up.
 *
 * No heap, no exceptions, no RTTI, no STL. Single precision throughout
 * except the calibration accumulators. Not reentrant: call from one
 * context, or guard with a critical section.
 */

//#pragma once

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
        float sigmaBias   = 0.02f;    /* bias random walk    [m/s^2/sqrt s] */

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

        /* gating */
        float cosTiltMin  = 0.819f;   /* reject lidar past 35 deg           */
        float nisGate     = 9.0f;     /* 3s chi-square on 1-D innovation    */
        uint32_t rejectsBeforeInflate = 40;

        /* stuck-lidar detector: a wedged TF02-Pro can keep reporting one
         * frozen distance forever with strength still in-range, which the
         * validity/NIS gates alone won't catch (it doesn't look invalid,
         * and if the vehicle isn't moving much it doesn't look surprising
         * either). Reject once the same post-decimation range repeats this
         * many blocks in a row. Default is ~0.5 s at the 200 Hz decimated
         * rate -- long enough that real near-hover motion essentially
         * never trips it, short enough to catch a multi-second wedge fast. */
        uint16_t frozenBlocksToReject = 5;

        /* free-fall plausibility gate: the vehicle cannot lose altitude
         * faster than gravity, so anything below that floor is a foreign
         * object in the beam rather than the ground. */
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
        uint32_t lidarImplausible    = 0;     /* free-fall gate rejections */
        uint32_t lidarBlocksDropped  = 0;     /* too few valid raw frames  */
        uint32_t lidarFrozen         = 0;     /* stuck-value rejections; NOT
                                                  folded into consecutiveRejects
                                                  -- a wedged sensor must never
                                                  be able to trigger inflate() */
        uint32_t consecutiveRejects  = 0;
        bool     lastUpdateAccepted  = false;

        float    baroInnovation      = 0.0f;  /* last y = z - h        [m] */
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
     * height, and zeroes the bias state.
     *
     * Only IMU is mandatory for finishCalibration() to succeed -- lidar and
     * baro are each independently optional. A missing/dead one just means
     * its own fusion stays disabled (pushLidarFrame()/pushBaroFrame()
     * become no-ops for height correction) and the initial height/
     * uncertainty falls back to whichever of the other did calibrate, or a
     * wide-open guess if neither did. A lidar that never shows up at boot
     * -- disconnected, dead, obstructed -- must not leave the filter stuck
     * in Calibrating forever with height()/velocity() silently frozen at 0
     * while the vehicle actually flies. Check
     * status().lidarCalibratedAtStart / baroCalibratedAtStart after a
     * successful call to know which reference you actually got. */
    void beginCalibration();
    bool finishCalibration();

    /* ---- run time -----------------------------------------------------
     * Both entry points are phase aware: during Calibrating they
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
     * applies an ungated scalar correction during Running. No-op if baro
     * wasn't part of the calibration window (see beginCalibration()).
     * Returns true if the filter was corrected. */
    bool pushBaroFrame(float pressurePa);

    /* Retune in flight, e.g. from your mission phase machine. */
    void setProcessNoise(float sigmaAccel, float sigmaTilt);

    /* Open the covariance back up after an inertial coast. */
    void inflate(float kPos, float kVel);

    /* ---- accessors ---------------------------------------------------- */
    float height()   const { return x_[0]; }
    float velocity() const { return x_[1]; }
    float bias()     const { return x_[2]; }
    float heightSigma()   const;
    float velocitySigma() const;
    float gravityRef() const { return gStatic_; }
    Phase phase()      const { return phase_; }
    const Status& status() const { return status_; }

private:
    static void  quatToR(const float q[4], float R[3][3]);
    static float bodyToWorldZ(const float q[4], const float v[3]);
    static void  mat3Mul(const float A[3][3], const float B[3][3], float C[3][3]);
    static void  mat3MulBt(const float A[3][3], const float B[3][3], float C[3][3]);

    /* Barometric formula against a FIXED reference (not local QNH) -- the
     * absolute reference cancels out via the pad-calibration subtraction
     * in pushBaroFrame(), so it doesn't need to be accurate, only stable. */
    static float pressureToHeight(float pressurePa);

    void symmetrise();
    bool projectLidar(float range, const float q[4],
                      float& z, float& cosTilt) const;
    void predict(float u, float dt);
    bool update(float range, const float q[4]);

    /* Shared scalar (H=[1 0 0]) correction math for both lidar and baro.
     * innovationStats() is pure (no state touched) so a caller can gate on
     * nis before deciding whether to call applyHeightCorrection() at all --
     * lidar needs that (reject past nisGate), baro doesn't (always applies,
     * its own sigmaBaro is the only thing limiting its influence). */
    static void innovationStats(float P00, float Rm, float y, float& S, float& nis);
    void applyHeightCorrection(float y, float Rm);

    Params  params_;
    Status  status_;
    Phase   phase_;

    float   x_[3];
    float   P_[3][3];
    float   gStatic_;

    /* free-fall gate anchor: last accepted state and its age */
    float    anchorH_;
    float    anchorV_;
    float    anchorTau_;
    bool     anchorValid_;

    /* decimator */
    uint32_t blockSum_;
    uint8_t  blockValid_;
    uint8_t  blockSeen_;

    /* stuck-lidar detector, see Params::frozenBlocksToReject */
    float    lastRange_;
    bool     lastRangeValid_;
    uint16_t frozenStreak_;

    /* calibration accumulators */
    double   calSumAz_;
    double   calSumH_;
    double   calSumBaro_;
    uint32_t calNAz_;
    uint32_t calNH_;
    uint32_t calNBaro_;

    /* baro pad baseline, see pushBaroFrame() */
    float    baroHeightBias_;
    bool     baroCalibrated_;
};
#endif /* ALTITUDEESTIMATOR_HPP_ */
