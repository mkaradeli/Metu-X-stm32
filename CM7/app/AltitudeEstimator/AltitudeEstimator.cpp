#include "AltitudeEstimator.hpp"

#include <cmath>
#include <cstring>

/* ==================================================================== */
/* linear algebra                                                       */
/* ==================================================================== */

void AltitudeEstimator::mat4Mul(const float A[4][4], const float B[4][4],
                                float C[4][4])
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k)
                s += A[i][k]*B[k][j];
            C[i][j] = s;
        }
}

/* C = A * B^T */
void AltitudeEstimator::mat4MulBt(const float A[4][4], const float B[4][4],
                                  float C[4][4])
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k)
                s += A[i][k]*B[j][k];
            C[i][j] = s;
        }
}

/* H P H^T, H a 1x4 row. */
float AltitudeEstimator::quadForm(const float P[4][4], const float H[4])
{
    float PHt[4];
    for (int i = 0; i < 4; ++i) {
        float s = 0.0f;
        for (int j = 0; j < 4; ++j)
            s += P[i][j]*H[j];
        PHt[i] = s;
    }
    float s = 0.0f;
    for (int i = 0; i < 4; ++i)
        s += H[i]*PHt[i];
    return s;
}

void AltitudeEstimator::quatToR(const float q[4], float R[3][3])
{
    const float w = q[0], x = q[1], y = q[2], z = q[3];
    const float xx = x*x, yy = y*y, zz = z*z;
    const float xy = x*y, xz = x*z, yz = y*z;
    const float wx = w*x, wy = w*y, wz = w*z;

    R[0][0] = 1.0f - 2.0f*(yy + zz);
    R[0][1] =        2.0f*(xy - wz);
    R[0][2] =        2.0f*(xz + wy);
    R[1][0] =        2.0f*(xy + wz);
    R[1][1] = 1.0f - 2.0f*(xx + zz);
    R[1][2] =        2.0f*(yz - wx);
    R[2][0] =        2.0f*(xz - wy);
    R[2][1] =        2.0f*(yz + wx);
    R[2][2] = 1.0f - 2.0f*(xx + yy);
}

/* World Z component of a body vector: the only row usually needed. */
float AltitudeEstimator::bodyToWorldZ(const float q[4], const float v[3])
{
    const float w = q[0], x = q[1], y = q[2], z = q[3];
    const float r20 = 2.0f*(x*z - w*y);
    const float r21 = 2.0f*(y*z + w*x);
    const float r22 = 1.0f - 2.0f*(x*x + y*y);
    return r20*v[0] + r21*v[1] + r22*v[2];
}

/* International barometric formula against a fixed reference pressure. The
 * reference doesn't need to match local QNH: any constant offset from using
 * a fixed reference is absorbed into the bb (baro bias) state. */
float AltitudeEstimator::pressureToHeight(float pressurePa)
{
    constexpr float kFixedRefPa = 101325.0f;
    if (pressurePa <= 0.0f)
        return 0.0f;
    return 44330.0f * (1.0f - std::pow(pressurePa / kFixedRefPa, 1.0f / 5.255f));
}

void AltitudeEstimator::symmetrise()
{
    for (int i = 0; i < 4; ++i)
        for (int j = i + 1; j < 4; ++j) {
            const float a = 0.5f*(P_[i][j] + P_[j][i]);
            P_[i][j] = P_[j][i] = a;
        }
}

bool AltitudeEstimator::isStateFinite() const
{
    for (int i = 0; i < 4; ++i)
        if (!std::isfinite(x_[i]))
            return false;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (!std::isfinite(P_[i][j]))
                return false;
    return true;
}

/* Force x_/P_ back to sane wide-open values -- same shape reset()/
 * finishCalibration() use when there's no calibrated reference to trust.
 * Re-anchors height to the last good lidar projection if one's available,
 * so the recovery isn't a jump to 0 while lidar is actively tracking. bb
 * (baro bias) is simply dropped and left to re-settle from subsequent
 * corrections -- there's no cheap "last known good" for it to fall back to
 * the way there is for h. */
void AltitudeEstimator::recoverFromNaN()
{
    ++status_.nanRecoveries;
    x_[0] = lastProjectedZValid_ ? lastProjectedZ_ : 0.0f;
    x_[1] = 0.0f;
    x_[2] = 0.0f;
    x_[3] = 0.0f;
    std::memset(P_, 0, sizeof(P_));
    P_[0][0] = 0.20f*0.20f;
    P_[1][1] = 0.50f*0.50f;
    P_[2][2] = 0.30f*0.30f;
    P_[3][3] = 1.00f*1.00f;
}

/* ==================================================================== */
/* lifecycle                                                            */
/* ==================================================================== */

void AltitudeEstimator::reset()
{
    phase_  = Phase::Idle;
    status_ = Status{};

    x_[0] = x_[1] = x_[2] = x_[3] = 0.0f;
    std::memset(P_, 0, sizeof(P_));
    P_[0][0] = 0.20f*0.20f;
    P_[1][1] = 0.50f*0.50f;
    P_[2][2] = 0.30f*0.30f;
    P_[3][3] = 1.00f*1.00f;   /* baro bias entirely unknown pre-calibration */

    gStatic_ = 9.80665f;

    anchorH_ = anchorV_ = anchorTau_ = 0.0f;
    anchorValid_ = false;

    lastProjectedZ_ = 0.0f;
    lastProjectedZValid_ = false;
    lastProjectedZNew_ = false;

    blockSum_ = 0; blockValid_ = 0; blockSeen_ = 0;
    calSumAz_ = 0.0; calSumH_ = 0.0; calSumBaro_ = 0.0;
    calNAz_ = 0; calNH_ = 0; calNBaro_ = 0;

    baroCalibrated_ = false;
}

void AltitudeEstimator::beginCalibration()
{
    calSumAz_ = 0.0; calSumH_ = 0.0; calSumBaro_ = 0.0;
    calNAz_ = 0; calNH_ = 0; calNBaro_ = 0;
    blockSum_ = 0; blockValid_ = 0; blockSeen_ = 0;
    phase_ = Phase::Calibrating;
}

bool AltitudeEstimator::finishCalibration()
{
    /* IMU is the one truly mandatory input: without a gravity reference,
     * predict() has nothing to integrate against, and nothing else in this
     * filter can substitute for it. Lidar and baro are each optional here
     * -- a disconnected/dead lidar (or baro) at boot must not leave the
     * filter stuck in Calibrating forever with height()/velocity() frozen
     * at 0, silently, while the vehicle is actually flying. */
    if (calNAz_ < kCalMinImu)
        return false;

    /* gStatic_ absorbs local gravity and accelerometer scale error, so the
     * bias state starts at zero and only has to track drift from here. */
    gStatic_ = static_cast<float>(calSumAz_ / static_cast<double>(calNAz_));

    const bool lidarCalibrated = (calNH_ >= kCalMinLidar);
    baroCalibrated_ = (calNBaro_ >= kCalMinBaro);
    status_.lidarCalibratedAtStart = lidarCalibrated;
    status_.baroCalibratedAtStart  = baroCalibrated_;

    /* h: prefer lidar's tighter baseline; with no lidar, the pad IS the
     * zero reference by definition, so h defaults to 0 rather than to
     * baro's (noisier, offset-prone) raw reading. */
    x_[0] = lidarCalibrated
          ? static_cast<float>(calSumH_ / static_cast<double>(calNH_))
          : 0.0f;
    x_[1] = 0.0f;
    x_[2] = 0.0f;

    /* bb = raw baro height - truth, truth being whatever h just got set to
     * above. With no lidar, that truth is the pad-zero default, so bb ends
     * up being baro's raw mean. */
    x_[3] = baroCalibrated_
          ? (static_cast<float>(calSumBaro_ / static_cast<double>(calNBaro_)) - x_[0])
          : 0.0f;

    std::memset(P_, 0, sizeof(P_));
    if (lidarCalibrated) {
        P_[0][0] = 0.02f*0.02f;
    } else if (baroCalibrated_) {
        P_[0][0] = params_.sigmaBaro * params_.sigmaBaro;
    } else {
        P_[0][0] = 0.20f*0.20f;   /* no calibrated height reference at all */
    }
    P_[1][1] = 0.05f*0.05f;
    P_[2][2] = 0.10f*0.10f;
    P_[3][3] = baroCalibrated_ ? (params_.sigmaBaro * params_.sigmaBaro)
                               : 1.00f*1.00f;

    status_.consecutiveRejects = 0;
    anchorH_ = x_[0]; anchorV_ = 0.0f; anchorTau_ = 0.0f; anchorValid_ = true;
    phase_ = Phase::Running;
    return true;
}

void AltitudeEstimator::setProcessNoise(float sigmaAccel, float sigmaTilt)
{
    params_.sigmaAccel = sigmaAccel;
    params_.sigmaTilt  = sigmaTilt;
}

void AltitudeEstimator::inflate(float kPos, float kVel)
{
    P_[0][0] *= kPos*kPos;
    P_[1][1] *= kVel*kVel;
    /* leave the bias covariances alone: neither the accel bias nor the baro
     * bias is something an inertial coast corrupts */
    symmetrise();
}

float AltitudeEstimator::heightSigma() const
{
    return (P_[0][0] > 0.0f) ? std::sqrt(P_[0][0]) : 0.0f;
}

float AltitudeEstimator::velocitySigma() const
{
    return (P_[1][1] > 0.0f) ? std::sqrt(P_[1][1]) : 0.0f;
}

/* ==================================================================== */
/* geometry                                                             */
/* ==================================================================== */

/* z = c*r - (R*lever)_z, with c = -(R*beam)_z. Returns false if the tilt
 * gate fails. */
bool AltitudeEstimator::projectLidar(float range, const float q[4],
                                     float& z, float& cosTilt) const
{
    float R[3][3];
    quatToR(q, R);

    const float nz = R[2][0]*params_.beam[0]
                   + R[2][1]*params_.beam[1]
                   + R[2][2]*params_.beam[2];
    cosTilt = -nz;

    /* Written as the ACCEPT condition, negated -- not "cosTilt < min" --
     * because any comparison against NaN is false in IEEE-754. A degenerate
     * quaternion (e.g. from a fast flip momentarily confusing the gyro
     * integrator) would silently sail through "cosTilt < min" (NaN < min is
     * false, so the reject never fires) and bake NaN straight into the
     * state forever. This form rejects NaN and out-of-[-1,1] cosTilt too. */
    if (!(cosTilt >= params_.cosTiltMin && cosTilt <= 1.0f))
        return false;

    const float lz = R[2][0]*params_.lever[0]
                   + R[2][1]*params_.lever[1]
                   + R[2][2]*params_.lever[2];

    z = cosTilt*range - lz;
    return true;
}

/* ==================================================================== */
/* predict                                                              */
/* ==================================================================== */

void AltitudeEstimator::predict(float u, float dt)
{
    if (!isStateFinite())
        recoverFromNaN();

    anchorTau_ += dt;

    x_[0] += x_[1]*dt + 0.5f*u*dt*dt;
    x_[1] += u*dt;
    /* x_[2] (accel bias) and x_[3] (baro bias) constant over the step */

    /* F = [ 1  dt  -dt^2/2  0 ;  0  1  -dt  0 ;  0  0  1  0 ;  0  0  0  1 ] */
    const float F[4][4] = {
        { 1.0f, dt,   -0.5f*dt*dt, 0.0f },
        { 0.0f, 1.0f, -dt,         0.0f },
        { 0.0f, 0.0f,  1.0f,       0.0f },
        { 0.0f, 0.0f,  0.0f,       1.0f }
    };

    float FP[4][4], Pn[4][4];
    mat4Mul(F, P_, FP);
    mat4MulBt(FP, F, Pn);

    const float sa2 = params_.sigmaAccel * params_.sigmaAccel;
    const float dt2 = dt*dt, dt3 = dt2*dt, dt4 = dt2*dt2;

    Pn[0][0] += sa2 * dt4 * 0.25f;
    Pn[0][1] += sa2 * dt3 * 0.5f;
    Pn[1][0] += sa2 * dt3 * 0.5f;
    Pn[1][1] += sa2 * dt2;
    Pn[2][2] += params_.sigmaBias * params_.sigmaBias * dt;
    Pn[3][3] += params_.sigmaBaroBiasWalk * params_.sigmaBaroBiasWalk * dt;

    std::memcpy(P_, Pn, sizeof(Pn));
    symmetrise();
}

void AltitudeEstimator::pushImu(const float aBody[3], const float q[4], float dt)
{
    if (phase_ == Phase::Calibrating) {
        const float azWCal = bodyToWorldZ(q, aBody);
        if (std::isfinite(azWCal)) {
            calSumAz_ += static_cast<double>(azWCal);
            ++calNAz_;
        }
        return;
    }
    if (phase_ != Phase::Running)
        return;

    if (dt <= 0.0f || dt > 0.1f)   /* reject bad timestamp deltas */
        return;

    const float azW = bodyToWorldZ(q, aBody);
    /* Same degenerate-quaternion risk as projectLidar()'s tilt gate, but
     * there's no threshold comparison here to (mis)protect it -- guard
     * explicitly instead of feeding a possible NaN/Inf straight into
     * predict() and corrupting the state permanently. */
    if (!std::isfinite(azW))
        return;
    const float u = azW - gStatic_ - x_[2];
    predict(u, dt);
}

/* ==================================================================== */
/* update                                                               */
/* ==================================================================== */

bool AltitudeEstimator::update(float range, const float q[4])
{
    static const float kHLidar[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

    if (!isStateFinite())
        recoverFromNaN();

    float z = 0.0f, cosTilt = 1.0f;
    status_.lastUpdateAccepted = false;

    if (!projectLidar(range, q, z, cosTilt)) {
        status_.cosTilt = cosTilt;
        ++status_.lidarRejected;
        ++status_.consecutiveRejects;
        /* Deliberately does NOT trigger inflate() here. Bad tilt is "no
         * data available right now", not evidence the filter's own state
         * is wrong -- unlike the NIS gate below, which inflates on genuine
         * statistical surprise. Sustained tilt (e.g. the vehicle sitting
         * flipped) would otherwise hit rejectsBeforeInflate every ~200ms
         * at the decimated lidar rate and inflate P00/P11 by 16x each
         * time, forever, with nothing to stop it -- not a one-time
         * re-open but unbounded runaway growth for as long as the tilt
         * persists. That blows the Kalman gain for every OTHER correction
         * source (baro included) up toward 1, making the fused output
         * track baro's raw noise almost unfiltered instead of smoothing
         * it -- the actual cause if height gets noisy while tilted. */
        return false;
    }
    status_.cosTilt = cosTilt;
    /* lastProjectedZ_ is captured further down, only once z has survived
     * the obstruction and NIS gates too -- see that assignment's comment.
     * Capturing it here (right after only the tilt check) would defeat
     * both gates for anything reading lastProjectedHeight() directly (e.g.
     * a controller bypassing the Kalman fusion on purpose): a cable
     * crossing the beam passes the tilt check fine, gets rejected by the
     * obstruction gate for the filter's own state, but would still flow
     * straight through to that raw accessor before the gate ever ran. */

    /* R = (sigma_r * c)^2 + r^2 (1 - c^2) sigma_theta^2
     * The second term is the attitude-uncertainty contribution: zero at
     * vertical, growing with both tilt and range. */
    float s2 = 1.0f - cosTilt*cosTilt;
    if (s2 < 0.0f) s2 = 0.0f;

    const float sr  = params_.sigmaRange0 + params_.sigmaRangeK * range;
    const float srv = sr * cosTilt;
    const float Rm  = srv*srv
                    + s2 * range * range * params_.sigmaTilt * params_.sigmaTilt;

    /* ---- obstruction gate -------------------------------------------
     * Project the last ACCEPTED lidar state forward under the hardest
     * physically possible descent (free fall under gravity alone, since
     * this vehicle's thrust is one-directional/upward) and refuse anything
     * below that floor. A cable, a ground umbilical, or a bird crossing
     * the beam produces an instantaneous metre-scale drop, which no
     * trajectory the vehicle itself could fly can match. Deliberately does
     * NOT feed consecutiveRejects: an impossible measurement must never be
     * able to trigger the covariance-opening recovery path, or a long
     * enough obstruction would be let through.
     *
     * Self-releasing by construction: the floor falls as 0.5*g*tau^2 while
     * the anchor goes unrefreshed, so a genuine descent the filter did not
     * predict is admitted after a few hundred milliseconds.
     *
     * Only valid while nothing but gravity and this vehicle's own upward
     * thrust can act on it -- a hand-shake bench test can push the sensor
     * down faster than gravity, so params_.freefallEnable should be off
     * for that kind of ground testing (see Params comment). */
    if (params_.freefallEnable && anchorValid_) {
        if (anchorTau_ > params_.freefallMaxTau) {
            anchorValid_ = false;      /* too stale to bound anything */
        } else {
            const float tau = anchorTau_;
            const float sh  = (P_[0][0] > 0.0f) ? std::sqrt(P_[0][0]) : 0.0f;
            const float sv  = (P_[1][1] > 0.0f) ? std::sqrt(P_[1][1]) : 0.0f;
            const float margin = params_.freefallMargin
                               + params_.freefallSigmaK * (sh + sv*tau);
            const float hFloor = anchorH_ + anchorV_*tau
                               - 0.5f*params_.freefallG*tau*tau - margin;
            if (z < hFloor) {
                ++status_.lidarImplausible;
                return false;
            }
        }
    }

    const float y = z - x_[0];
    float S = 0.0f, nis = 0.0f;
    innovationStats(quadForm(P_, kHLidar), Rm, y, S, nis);
    if (S <= 0.0f)
        return false;

    status_.innovation = y;
    status_.nis        = nis;

    /* Loose plausibility gate -- see Params::nisGate comment. Only meant to
     * catch genuinely implausible single readings, not to second-guess
     * real fast dynamics (the obstruction gate above already handles the
     * specific "something crossed the beam" case more precisely). */
    if (nis > params_.nisGate) {
        ++status_.lidarRejected;
        ++status_.consecutiveRejects;
        if (status_.consecutiveRejects > params_.rejectsBeforeInflate) {
            inflate(4.0f, 4.0f);
            status_.consecutiveRejects = 0;
        }
        return false;
    }

    /* z has now passed tilt, obstruction, and NIS -- safe to expose as the
     * "raw but validated" projected height. */
    lastProjectedZ_ = z;
    lastProjectedZValid_ = true;
    lastProjectedZNew_ = true;

    applyCorrection(kHLidar, y, Rm);

    status_.consecutiveRejects = 0;
    ++status_.lidarAccepted;
    status_.lastUpdateAccepted = true;

    anchorH_ = x_[0]; anchorV_ = x_[1]; anchorTau_ = 0.0f; anchorValid_ = true;
    return true;
}

/* Pure: computes S and nis for a would-be correction without touching
 * state, so a caller can decide whether to apply it at all. */
void AltitudeEstimator::innovationStats(float HPHt, float Rm, float y,
                                        float& S, float& nis)
{
    S   = HPHt + Rm;
    nis = (S > 0.0f) ? (y*y) / S : 0.0f;
}

/* Applies the Kalman correction for an arbitrary 1x4 H unconditionally --
 * caller must have already decided to accept (checked S > 0, any gates). */
void AltitudeEstimator::applyCorrection(const float H[4], float y, float Rm)
{
    float PHt[4];
    for (int i = 0; i < 4; ++i) {
        float s = 0.0f;
        for (int j = 0; j < 4; ++j)
            s += P_[i][j]*H[j];
        PHt[i] = s;
    }

    float S = Rm;
    for (int i = 0; i < 4; ++i)
        S += H[i]*PHt[i];
    if (S <= 0.0f)
        return;

    float K[4];
    for (int i = 0; i < 4; ++i)
        K[i] = PHt[i]/S;

    for (int i = 0; i < 4; ++i)
        x_[i] += K[i]*y;

    /* Joseph form: P = A P A' + K Rm K',  A = I - K H */
    float A[4][4];
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            A[i][j] = (i == j ? 1.0f : 0.0f) - K[i]*H[j];

    float AP[4][4], Pn[4][4];
    mat4Mul(A, P_, AP);
    mat4MulBt(AP, A, Pn);

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            Pn[i][j] += K[i]*Rm*K[j];

    std::memcpy(P_, Pn, sizeof(Pn));
    symmetrise();
}

bool AltitudeEstimator::pushBaroFrame(float pressurePa)
{
    if (phase_ == Phase::Idle)
        return false;

    const float h = pressureToHeight(pressurePa);

    if (phase_ == Phase::Calibrating) {
        calSumBaro_ += static_cast<double>(h);
        ++calNBaro_;
        return false;
    }
    if (phase_ != Phase::Running || !baroCalibrated_)
        return false;

    if (!isStateFinite())
        recoverFromNaN();

    /* Raw baro height = h + bb by construction, so H = [1 0 0 1]. */
    static const float kHBaro[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    const float y  = h - (x_[0] + x_[3]);
    const float Rm = params_.sigmaBaro * params_.sigmaBaro;

    float S = 0.0f, nis = 0.0f;
    innovationStats(quadForm(P_, kHBaro), Rm, y, S, nis);
    if (S <= 0.0f)
        return false;

    status_.baroInnovation = y;
    status_.baroNis        = nis;

    /* No rejection gate, deliberately -- see the class-level comment. */
    applyCorrection(kHBaro, y, Rm);

    ++status_.baroAccepted;
    status_.lastBaroUpdateAccepted = true;
    return true;
}

bool AltitudeEstimator::pushLidarFrame(uint16_t distMm, uint16_t strength,
                                       const float q[4])
{
    if (phase_ == Phase::Idle)
        return false;

    /* Validity gate. The sensor forces dist to its maximum (4500 cm) when
     * strength < 60, and to 65534 when the return saturates: both are
     * status codes, not measurements. */
    const bool ok = (strength >= kStrengthMin)
                 && (strength <  kStrengthSat)
                 && (distMm   <  kDistMaxMm)
                 && (distMm   >= kDistMinMm);

    if (ok) { blockSum_ += distMm; ++blockValid_; }
    ++blockSeen_;

    if (blockSeen_ < kLidarBlock)
        return false;

    const uint8_t  nValid = blockValid_;
    const uint32_t sum    = blockSum_;
    blockSum_ = 0; blockValid_ = 0; blockSeen_ = 0;

    if (nValid < kLidarMinValid) {
        ++status_.lidarBlocksDropped;
        return false;
    }

    /* Mean of the valid samples. Corresponds in time to the CENTRE of the
     * block, i.e. ~2.5 ms before this call at a 1 kHz frame rate. */
    const float range = static_cast<float>(sum)
                      / static_cast<float>(nValid) * 0.001f;

    if (phase_ == Phase::Calibrating) {
        float z = 0.0f, c = 1.0f;
        if (projectLidar(range, q, z, c)) {
            calSumH_ += static_cast<double>(z);
            ++calNH_;
        }
        return false;
    }

    return update(range, q);
}
