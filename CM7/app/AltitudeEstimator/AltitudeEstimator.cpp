#include "AltitudeEstimator.hpp"

#include <cmath>
#include <cstring>

/* ==================================================================== */
/* linear algebra                                                       */
/* ==================================================================== */

void AltitudeEstimator::mat3Mul(const float A[3][3], const float B[3][3],
                                float C[3][3])
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            C[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j];
}

/* C = A * B^T */
void AltitudeEstimator::mat3MulBt(const float A[3][3], const float B[3][3],
                                  float C[3][3])
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            C[i][j] = A[i][0]*B[j][0] + A[i][1]*B[j][1] + A[i][2]*B[j][2];
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

void AltitudeEstimator::symmetrise()
{
    const float a = 0.5f*(P_[0][1] + P_[1][0]); P_[0][1] = P_[1][0] = a;
    const float b = 0.5f*(P_[0][2] + P_[2][0]); P_[0][2] = P_[2][0] = b;
    const float c = 0.5f*(P_[1][2] + P_[2][1]); P_[1][2] = P_[2][1] = c;
}

bool AltitudeEstimator::isStateFinite() const
{
    if (!std::isfinite(x_[0]) || !std::isfinite(x_[1]) || !std::isfinite(x_[2]))
        return false;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (!std::isfinite(P_[i][j]))
                return false;
    return true;
}

/* Force x_/P_ back to sane wide-open values -- same shape reset()/
 * finishCalibration() use when there's no calibrated reference to trust.
 * Re-anchors height to the last good lidar projection if one's available,
 * so the recovery isn't a jump to 0 while lidar is actively tracking. */
void AltitudeEstimator::recoverFromNaN()
{
    ++status_.nanRecoveries;
    x_[0] = lastProjectedZValid_ ? lastProjectedZ_ : 0.0f;
    x_[1] = 0.0f;
    x_[2] = 0.0f;
    std::memset(P_, 0, sizeof(P_));
    P_[0][0] = 0.20f*0.20f;
    P_[1][1] = 0.50f*0.50f;
    P_[2][2] = 0.30f*0.30f;
}

/* ==================================================================== */
/* lifecycle                                                            */
/* ==================================================================== */

void AltitudeEstimator::reset()
{
    phase_  = Phase::Idle;
    status_ = Status{};

    x_[0] = x_[1] = x_[2] = 0.0f;
    std::memset(P_, 0, sizeof(P_));
    P_[0][0] = 0.20f*0.20f;
    P_[1][1] = 0.50f*0.50f;
    P_[2][2] = 0.30f*0.30f;

    gStatic_ = 9.80665f;

    lastProjectedZ_ = 0.0f;
    lastProjectedZValid_ = false;
    lastProjectedZNew_ = false;

    blockSum_ = 0; blockValid_ = 0; blockSeen_ = 0;
    calSumAz_ = 0.0; calSumH_ = 0.0; calNAz_ = 0; calNH_ = 0;
}

void AltitudeEstimator::beginCalibration()
{
    calSumAz_ = 0.0; calSumH_ = 0.0; calNAz_ = 0; calNH_ = 0;
    blockSum_ = 0; blockValid_ = 0; blockSeen_ = 0;
    phase_ = Phase::Calibrating;
}

bool AltitudeEstimator::finishCalibration()
{
    if (calNAz_ < kCalMinImu || calNH_ < kCalMinLidar)
        return false;

    /* gStatic_ absorbs local gravity and accelerometer scale error, so the
     * bias state starts at zero and only has to track drift from here. */
    gStatic_ = static_cast<float>(calSumAz_ / static_cast<double>(calNAz_));

    x_[0] = static_cast<float>(calSumH_ / static_cast<double>(calNH_));
    x_[1] = 0.0f;
    x_[2] = 0.0f;

    std::memset(P_, 0, sizeof(P_));
    P_[0][0] = 0.02f*0.02f;
    P_[1][1] = 0.05f*0.05f;
    P_[2][2] = 0.10f*0.10f;

    status_.consecutiveRejects = 0;
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
    /* leave the bias covariance alone: it is the one thing an inertial
     * coast did not corrupt */
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

    x_[0] += x_[1]*dt + 0.5f*u*dt*dt;
    x_[1] += u*dt;
    /* x_[2] constant over the step */

    /* F = [ 1  dt  -dt^2/2 ;  0  1  -dt ;  0  0  1 ] */
    const float F[3][3] = {
        { 1.0f, dt,   -0.5f*dt*dt },
        { 0.0f, 1.0f, -dt         },
        { 0.0f, 0.0f,  1.0f       }
    };

    float FP[3][3], Pn[3][3];
    mat3Mul(F, P_, FP);
    mat3MulBt(FP, F, Pn);

    const float sa2 = params_.sigmaAccel * params_.sigmaAccel;
    const float dt2 = dt*dt, dt3 = dt2*dt, dt4 = dt2*dt2;

    Pn[0][0] += sa2 * dt4 * 0.25f;
    Pn[0][1] += sa2 * dt3 * 0.5f;
    Pn[1][0] += sa2 * dt3 * 0.5f;
    Pn[1][1] += sa2 * dt2;
    Pn[2][2] += params_.sigmaBias * params_.sigmaBias * dt;

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
    if (!isStateFinite())
        recoverFromNaN();

    float z = 0.0f, cosTilt = 1.0f;
    status_.lastUpdateAccepted = false;

    if (!projectLidar(range, q, z, cosTilt)) {
        status_.cosTilt = cosTilt;
        ++status_.lidarRejected;
        ++status_.consecutiveRejects;
        /* Sustained bad tilt geometry means the filter is being denied
         * corrections for real (not statistical-surprise) reasons; still
         * worth re-opening the covariance once it clears. */
        if (status_.consecutiveRejects > params_.rejectsBeforeInflate) {
            inflate(4.0f, 4.0f);
            status_.consecutiveRejects = 0;
        }
        return false;
    }
    status_.cosTilt = cosTilt;
    lastProjectedZ_ = z;
    lastProjectedZValid_ = true;
    lastProjectedZNew_ = true;

    /* R = (sigma_r * c)^2 + r^2 (1 - c^2) sigma_theta^2
     * The second term is the attitude-uncertainty contribution: zero at
     * vertical, growing with both tilt and range. */
    float s2 = 1.0f - cosTilt*cosTilt;
    if (s2 < 0.0f) s2 = 0.0f;

    const float sr  = params_.sigmaRange0 + params_.sigmaRangeK * range;
    const float srv = sr * cosTilt;
    const float Rm  = srv*srv
                    + s2 * range * range * params_.sigmaTilt * params_.sigmaTilt;

    const float y = z - x_[0];
    const float S = P_[0][0] + Rm;
    if (S <= 0.0f)
        return false;

    const float nis = (y*y) / S;
    status_.innovation = y;
    status_.nis        = nis;

    /* Loose plausibility gate -- see Params::nisGate comment. Only meant to
     * catch genuinely implausible single readings, not to second-guess
     * real fast dynamics. */
    if (nis > params_.nisGate) {
        ++status_.lidarRejected;
        ++status_.consecutiveRejects;
        if (status_.consecutiveRejects > params_.rejectsBeforeInflate) {
            inflate(4.0f, 4.0f);
            status_.consecutiveRejects = 0;
        }
        return false;
    }

    const float K[3] = { P_[0][0]/S, P_[1][0]/S, P_[2][0]/S };

    x_[0] += K[0]*y;
    x_[1] += K[1]*y;
    x_[2] += K[2]*y;

    /* Joseph form: P = A P A' + K Rm K',  A = I - K H,  H = [1 0 0] */
    const float A[3][3] = {
        { 1.0f - K[0], 0.0f, 0.0f },
        {      -K[1],  1.0f, 0.0f },
        {      -K[2],  0.0f, 1.0f }
    };
    float AP[3][3], Pn[3][3];
    mat3Mul(A, P_, AP);
    mat3MulBt(AP, A, Pn);

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            Pn[i][j] += K[i]*Rm*K[j];

    std::memcpy(P_, Pn, sizeof(Pn));
    symmetrise();

    status_.consecutiveRejects = 0;
    ++status_.lidarAccepted;
    status_.lastUpdateAccepted = true;
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
