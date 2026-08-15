#include "AltitudeKf.hpp"

#include <cmath>

namespace gnc {

namespace {
constexpr float kGravity = 9.80665f;
constexpr float kMinTiltCos = 1.0e-3f;   // numerical floor on cos(theta)
}  // namespace

// ---------------------------------------------------------------------------

float AltitudeKf::safeSqrt(float v) noexcept
{
    return (v > 0.0f) ? std::sqrt(v) : 0.0f;
}

void AltitudeKf::forceSymmetry() noexcept
{
    float t;
    t = 0.5f * (P_[0][1] + P_[1][0]); P_[0][1] = P_[1][0] = t;
    t = 0.5f * (P_[0][2] + P_[2][0]); P_[0][2] = P_[2][0] = t;
    t = 0.5f * (P_[1][2] + P_[2][1]); P_[1][2] = P_[2][1] = t;
}

// ---------------------------------------------------------------------------

void AltitudeKf::init(float h0, float b0,
                      float sigmaH, float sigmaV, float sigmaB) noexcept
{
    x_[0] = h0;
    x_[1] = 0.0f;
    x_[2] = b0;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) { P_[i][j] = 0.0f; }
    }
    P_[0][0] = sigmaH * sigmaH;
    P_[1][1] = sigmaV * sigmaV;
    P_[2][2] = sigmaB * sigmaB;

    upRow_  = Vec3{0.0f, 0.0f, 1.0f};
    c_      = 1.0f;
    cz_     = 1.0f;
    lambda_ = 0.0f;

    lastNu_      = 0.0f;
    lastNis_     = 0.0f;
    rejects_     = 0;
    lastResult_  = UpdateResult::Accepted;
    haveStamp_   = false;
    haveStampUs_ = false;
    initialised_ = true;
}

void AltitudeKf::reset() noexcept
{
    initialised_ = false;
    haveStamp_   = false;
    haveStampUs_ = false;
    lastResult_  = UpdateResult::NotInitialised;
}

// ---------------------------------------------------------------------------
// time update
// ---------------------------------------------------------------------------

bool AltitudeKf::predict(float dt, const Vec3& specificForce,
                         const Quat& q) noexcept
{
    if (!initialised_)              { return false; }
    if (dt <= 0.0f || dt > cfg_.maxDt) { return false; }

    // ---- attitude coupling: three dot products, no full DCM ----
    upRow_ = q.upRow();

    float c = -upRow_.dot(cfg_.lidarBeam);
    if (c < kMinTiltCos) { c = kMinTiltCos; }
    c_      = c;
    cz_     = upRow_.z;                 // (R * e_z_body) projected on world Up
    lambda_ = upRow_.dot(cfg_.lidarPos);

    // ---- vertical specific force ----
    // The bias lives on body z, so its vertical contribution is scaled by cz.
    const float aUp = upRow_.dot(specificForce) - cz_ * x_[2] - kGravity;

    // ---- state propagation ----
    const float dt2 = dt * dt;
    x_[0] += x_[1] * dt + 0.5f * aUp * dt2;
    x_[1] += aUp * dt;
    // x_[2] is constant under the model

    // ---- covariance: P = F P F' + Q ----
    // F = [ 1  dt  -0.5*cz*dt^2 ]
    //     [ 0   1  -cz*dt       ]
    //     [ 0   0   1           ]
    const float f02 = -0.5f * cz_ * dt2;
    const float f12 = -cz_ * dt;

    float M[3][3];
    for (int j = 0; j < 3; ++j) {                       // M = F * P
        M[0][j] = P_[0][j] + dt * P_[1][j] + f02 * P_[2][j];
        M[1][j] =            P_[1][j] + f12 * P_[2][j];
        M[2][j] =                             P_[2][j];
    }
    for (int i = 0; i < 3; ++i) {                       // P = M * F'
        const float m0 = M[i][0], m1 = M[i][1], m2 = M[i][2];
        P_[i][0] = m0 + dt * m1 + f02 * m2;
        P_[i][1] =           m1 + f12 * m2;
        P_[i][2] =                     m2;
    }

    // Exact van Loan discretisation for a double integrator driven by white
    // accel noise, plus a random-walk bias entering with gain -1:
    //
    //   Q = qA * [ dt^3/3   dt^2/2    0      ]
    //            [ dt^2/2   dt        0      ]
    //            [ 0        0         0      ]
    //     + qB * [ dt^5/20  dt^4/8   -dt^3/6 ]
    //            [ dt^4/8   dt^3/3   -dt^2/2 ]
    //            [-dt^3/6  -dt^2/2    dt     ]
    //
    // The negative cross terms are correct; they are what makes the bias
    // observable in reasonable time.
    const float dt3 = dt2 * dt;
    const float dt4 = dt3 * dt;
    const float dt5 = dt4 * dt;
    const float qA = cfg_.qAccel;
    const float qB = cfg_.qBias;

    P_[0][0] += qA * dt3 / 3.0f  + qB * dt5 / 20.0f;
    P_[0][1] += qA * dt2 * 0.5f  + qB * dt4 / 8.0f;
    P_[0][2] +=                  - qB * dt3 / 6.0f;
    P_[1][1] += qA * dt          + qB * dt3 / 3.0f;
    P_[1][2] +=                  - qB * dt2 * 0.5f;
    P_[2][2] +=                    qB * dt;

    P_[1][0] = P_[0][1];
    P_[2][0] = P_[0][2];
    P_[2][1] = P_[1][2];

    forceSymmetry();
    return true;
}

bool AltitudeKf::predictAt(uint32_t cycleCount, const Vec3& specificForce,
                           const Quat& q) noexcept
{
    if (!haveStamp_) {
        lastStamp_ = cycleCount;
        haveStamp_ = true;
        return false;                        // seed only
    }
    const uint32_t elapsed = cycleCount - lastStamp_;   // wraps correctly
    lastStamp_ = cycleCount;

    const float dt = static_cast<float>(elapsed) / cycleRate_;
    return predict(dt, specificForce, q);
}

bool AltitudeKf::predictAtMicros(uint64_t nowUs, const Vec3& specificForce,
                                 const Quat& q) noexcept
{
    if (!haveStampUs_) {
        lastStampUs_ = nowUs;
        haveStampUs_ = true;
        return false;                        // seed only
    }
    if (nowUs <= lastStampUs_) {             // non-monotonic read, e.g. a torn
        lastStampUs_ = nowUs;                // 64-bit timestamp; skip this step
        return false;
    }
    const uint64_t elapsedUs = nowUs - lastStampUs_;
    lastStampUs_ = nowUs;

    const float dt = static_cast<float>(elapsedUs) * 1.0e-6f;
    return predict(dt, specificForce, q);    // maxDt guard catches torn reads
}

// ---------------------------------------------------------------------------
// lidar measurement update
// ---------------------------------------------------------------------------

AltitudeKf::UpdateResult AltitudeKf::updateLidar(float range) noexcept
{
    const float frac = cfg_.sigmaRangeFrac * range;
    const float varRange = cfg_.sigmaRangeFloor * cfg_.sigmaRangeFloor
                         + frac * frac;
    return applyRange(range, varRange);
}

AltitudeKf::UpdateResult AltitudeKf::updateLidar(float range,
                                                 uint16_t strength) noexcept
{
    // Benewake TF-series convention: a weak return, or the 65535 saturation
    // code, means the distance field is not trustworthy. Verify the exact
    // semantics against your TF02-Pro datasheet.
    if (strength < cfg_.minStrength || strength == 65535u) {
        ++rejects_;
        lastResult_ = UpdateResult::WeakSignal;
        return lastResult_;
    }

    // Range precision scales roughly as 1/sqrt(SNR), so:
    //     var_r = floor^2 + k / strength
    // Calibrate k on a static rig: sweep range over your landing envelope on
    // the surfaces you actually land on, log ~1000 samples per condition,
    // then least-squares fit sample variance against 1/strength. The slope is
    // kStrength, the intercept is sigmaRangeFloor^2. If the fit is poor, bin
    // by strength decile and use a lookup table instead.
    const float varRange = cfg_.sigmaRangeFloor * cfg_.sigmaRangeFloor
                         + cfg_.kStrength / static_cast<float>(strength);
    return applyRange(range, varRange);
}

AltitudeKf::UpdateResult AltitudeKf::applyRange(float range,
                                                float varRange) noexcept
{
    if (!initialised_) {
        lastResult_ = UpdateResult::NotInitialised;
        return lastResult_;
    }
    if (range < cfg_.minRange || range > cfg_.maxRange) {
        ++rejects_;
        lastResult_ = UpdateResult::RangeOutOfBounds;
        return lastResult_;
    }
    if (c_ < cfg_.minTiltCos) {
        ++rejects_;
        lastResult_ = UpdateResult::TiltTooLarge;
        return lastResult_;
    }

    const float cInv = 1.0f / c_;

    // Predicted slant range: r_hat = (h + lambda) / c.
    // Predicting the RANGE rather than pre-correcting the measurement to an
    // altitude keeps the lidar noise in its native coordinate.
    const float rHat = (x_[0] + lambda_) * cInv;
    const float nu   = range - rHat;

    // Attitude error contributes  dr/dtheta = r * tan(theta),  which vanishes
    // at theta = 0. The geometry is second-order insensitive to tilt error
    // while upright, and degrades gracefully as the vehicle leans.
    const float sinSq = 1.0f - c_ * c_;
    const float tanT  = safeSqrt(sinSq) * cInv;
    const float eAtt  = range * tanT * cfg_.sigmaTilt;
    const float varR  = varRange + eAtt * eAtt;

    // H = [1/c, 0, 0]
    const float S = P_[0][0] * cInv * cInv + varR;
    if (!(S > 0.0f)) {
        lastResult_ = UpdateResult::Singular;
        return lastResult_;
    }

    const float nis = nu * nu / S;
    lastNu_  = nu;
    lastNis_ = nis;
    if (nis > cfg_.nisGate) {
        ++rejects_;
        lastResult_ = UpdateResult::NisGated;
        return lastResult_;
    }

    const float g  = cInv / S;
    const float K0 = P_[0][0] * g;
    const float K1 = P_[1][0] * g;
    const float K2 = P_[2][0] * g;

    x_[0] += K0 * nu;
    x_[1] += K1 * nu;
    x_[2] += K2 * nu;

    // Joseph form. Worth the cycles: the short form loses symmetry quickly in
    // float32 at these update rates.
    const float A[3][3] = {
        {1.0f - K0 * cInv, 0.0f, 0.0f},
        {     - K1 * cInv, 1.0f, 0.0f},
        {     - K2 * cInv, 0.0f, 1.0f}
    };
    const float K[3] = {K0, K1, K2};

    float T[3][3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            T[i][j] = A[i][0] * P_[0][j] + A[i][1] * P_[1][j] + A[i][2] * P_[2][j];
        }
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            P_[i][j] = T[i][0] * A[j][0] + T[i][1] * A[j][1] + T[i][2] * A[j][2]
                     + K[i] * varR * K[j];
        }
    }

    forceSymmetry();
    lastResult_ = UpdateResult::Accepted;
    return lastResult_;
}

// ---------------------------------------------------------------------------
// zero-velocity update
// ---------------------------------------------------------------------------

void AltitudeKf::updateZeroVelocity(float sigmaV) noexcept
{
    if (!initialised_) { return; }

    const float varV = sigmaV * sigmaV;
    const float S    = P_[1][1] + varV;
    if (!(S > 0.0f)) { return; }

    const float K0 = P_[0][1] / S;
    const float K1 = P_[1][1] / S;
    const float K2 = P_[2][1] / S;

    const float nu = 0.0f - x_[1];
    x_[0] += K0 * nu;
    x_[1] += K1 * nu;
    x_[2] += K2 * nu;

    const float A[3][3] = {
        {1.0f, -K0,        0.0f},
        {0.0f, 1.0f - K1,  0.0f},
        {0.0f, -K2,        1.0f}
    };
    const float K[3] = {K0, K1, K2};

    float T[3][3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            T[i][j] = A[i][0] * P_[0][j] + A[i][1] * P_[1][j] + A[i][2] * P_[2][j];
        }
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            P_[i][j] = T[i][0] * A[j][0] + T[i][1] * A[j][1] + T[i][2] * A[j][2]
                     + K[i] * varV * K[j];
        }
    }

    forceSymmetry();
}

}  // namespace gnc
