/* HilNav.cpp -- the altitude filter as plain C. See HilNav.h for how to drive it; its settings are the
 * compile-time constants in HilNavConfig.h.
 *
 * VALID C99 AND VALID C++. The repository builds it as C++ with everything else; renamed to HilNav.c it builds
 * unchanged with a C compiler, and the test build does exactly that to keep the claim true. So: <math.h> only,
 * no heap, no globals, every helper `static` and prefixed `hn_` so it cannot collide in a unity build.
 *
 * EVERY LINE OF ARITHMETIC IS A COPY. The filter is AltitudeEstimator.cpp and the matrix work KalmanFilter.cpp,
 * operation for operation and loop for loop, because the test holding the two together compares bits, not
 * tolerances -- rewriting `a*b + c` as `c + a*b` here would be a failure there. Change them together. */

#include <math.h>

#include "HilNav.h"
#include "HilNavConfig.h"

enum { HN_N = HILNAV_STATES };

static const double hn_g = 9.80665;
static const double hn_HLidar[HN_N] = { 1.0, 0.0, 0.0, 0.0, 0.0 };
static const double hn_HAccel[HN_N] = { 0.0, 0.0, 1.0, 0.0, 0.0 };
static const double hn_HVel[HN_N]   = { 0.0, 1.0, 0.0, 0.0, 0.0 };

typedef struct hn_outcome { double Innovation, Variance, Nis; int Accepted; } hn_outcome;

static double hn_sq(double X) { return X * X; }
static double hn_max(double A, double B) { return (A < B) ? B : A; }     /* std::max */
static double hn_min(double A, double B) { return (B < A) ? B : A; }     /* std::min */

/* The same ISA inversion the barometer applies, for the local metres-per-pascal. */
static double hn_isa_altitude(double PPa) {
    const double T0 = 288.15, L = 0.0065, R = 287.052874, g = 9.80665, P0Pa = 101325.0;
    if (PPa <= 0.0) return 0.0;
    return T0 / L * (1.0 - pow(PPa / P0Pa, R * L / g));
}

/* ---- the Kalman filter: KalmanFilter.cpp at a fixed five states ------------------------------------------ */

static void hn_kf_reset(HilNav* Nav, const double* X0, const double* P0Diag) {
    int i;
    for (i = 0; i < HN_N; ++i) Nav->X[i] = 0.0;
    for (i = 0; i < HN_N * HN_N; ++i) Nav->P[i] = 0.0;
    if (X0 != 0) for (i = 0; i < HN_N; ++i) Nav->X[i] = X0[i];
    if (P0Diag != 0) for (i = 0; i < HN_N; ++i) Nav->P[i * HN_N + i] = P0Diag[i];
}

static void hn_set_cov(HilNav* Nav, int I, int J, double V) {
    Nav->P[I * HN_N + J] = V;
    Nav->P[J * HN_N + I] = V;
}

static double hn_sigma(const HilNav* Nav, int I) {
    const double v = Nav->P[I * HN_N + I];
    return (v > 0.0) ? sqrt(v) : 0.0;
}

static void hn_symmetrize(HilNav* Nav) {
    int i, j;
    for (i = 0; i < HN_N; ++i)
        for (j = i + 1; j < HN_N; ++j) {
            const double m = 0.5 * (Nav->P[i * HN_N + j] + Nav->P[j * HN_N + i]);
            Nav->P[i * HN_N + j] = m;
            Nav->P[j * HN_N + i] = m;
        }
}

/* x <- F x, P <- F P F' + Q */
static void hn_kf_predict(HilNav* Nav, const double* F, const double* Q) {
    double xs[HN_N], t[HN_N * HN_N];
    int i, j, k;
    for (i = 0; i < HN_N; ++i) {
        double s = 0.0;
        for (j = 0; j < HN_N; ++j) s += F[i * HN_N + j] * Nav->X[j];
        xs[i] = s;
    }
    for (i = 0; i < HN_N; ++i) Nav->X[i] = xs[i];
    for (i = 0; i < HN_N; ++i)
        for (j = 0; j < HN_N; ++j) {
            double s = 0.0;
            for (k = 0; k < HN_N; ++k) s += F[i * HN_N + k] * Nav->P[k * HN_N + j];
            t[i * HN_N + j] = s;
        }
    for (i = 0; i < HN_N; ++i)
        for (j = 0; j < HN_N; ++j) {
            double s = 0.0;
            for (k = 0; k < HN_N; ++k) s += t[i * HN_N + k] * F[j * HN_N + k];
            Nav->P[i * HN_N + j] = s + ((Q != 0) ? Q[i * HN_N + j] : 0.0);
        }
    hn_symmetrize(Nav);
}

/* One scalar measurement, gated on NIS, Joseph form written out. A rejection changes nothing. */
static hn_outcome hn_kf_update(HilNav* Nav, const double* H, double Z, double R, double GateNis) {
    hn_outcome o;
    double ph[HN_N], kg[HN_N];
    double s = R, hx = 0.0;
    int i, j;
    o.Innovation = 0.0; o.Variance = 0.0; o.Nis = 0.0; o.Accepted = 0;

    for (i = 0; i < HN_N; ++i) {
        double v = 0.0;
        for (j = 0; j < HN_N; ++j) v += Nav->P[i * HN_N + j] * H[j];
        ph[i] = v;
        s += H[i] * v;
    }
    o.Variance = s;
    if (!(s > 0.0)) return o;

    for (i = 0; i < HN_N; ++i) hx += H[i] * Nav->X[i];
    o.Innovation = Z - hx;
    o.Nis = o.Innovation * o.Innovation / s;
    if (GateNis > 0.0 && o.Nis > GateNis) return o;
    o.Accepted = 1;

    for (i = 0; i < HN_N; ++i) kg[i] = ph[i] / s;
    for (i = 0; i < HN_N; ++i) Nav->X[i] += kg[i] * o.Innovation;
    for (i = 0; i < HN_N; ++i)
        for (j = 0; j < HN_N; ++j)
            Nav->P[i * HN_N + j] += -kg[i] * ph[j] - ph[i] * kg[j] + s * kg[i] * kg[j];
    hn_symmetrize(Nav);
    return o;
}

/* ---- the altitude filter: AltitudeEstimator.cpp ------------------------------------------------------------ */

/* THE SETTINGS: HilNavConfig.h's constants in one read-only table, built at compile time and read the way
 * AltitudeEstimator reads its Config -- so the arithmetic below stays that file's, line for line. */
typedef struct hn_settings {
    double JerkPsd, BiasSigmaM, BiasTauS;
    int    UseLidar;
    double LidarSigmaM;
    int    LidarStrengthThreshold;
    double LidarLsbM, LidarRangeMinM, LidarRangeMaxM, MountBelowCgM;
    double TiltSigmaRad, LidarMaxTiltRad, LidarGateNis;
    int    UseBaro;
    double BaroTauVentS;
    int    BaroIirCoeff;
    double BaroOdrHz, BaroSigmaPa, BaroLsbPa, BaroModelSigmaM;
    int    UseBaroLagModel;
    double BaroGateNis;
    int    UseAccel;
    double AccelSigmaMps2, AccelTiltSigmaRad, AccelGateNis;
    int    AccelReacquireAfter;
    double AccelOffsetMps2;
    int    UseImuTilt, UseStationary;
    double StationaryRateRadS, StationaryAccelMps2, StationaryHoldS, StationaryMaxRangeM;
    double ZeroVelocitySigma, OffsetLearnTauS;
    int    ReacquireAfterRejects;
    double InitVelSigma, InitAccSigma, InitAccel;
} hn_settings;

static const hn_settings hn_cfg = {
    HILNAV_JERK_PSD, HILNAV_BIAS_SIGMA_M, HILNAV_BIAS_TAU_S,
    HILNAV_USE_LIDAR,
    HILNAV_LIDAR_SIGMA_M,
    HILNAV_LIDAR_STRENGTH_THRESHOLD,
    HILNAV_LIDAR_LSB_M, HILNAV_LIDAR_RANGE_MIN_M, HILNAV_LIDAR_RANGE_MAX_M,
    HILNAV_REF_Z - HILNAV_LIDAR_MOUNT_Z,              /* the mount's depth below the reported point */
    HILNAV_TILT_SIGMA_RAD, HILNAV_LIDAR_MAX_TILT_RAD, HILNAV_LIDAR_GATE_NIS,
    HILNAV_USE_BARO,
    HILNAV_BARO_TAU_VENT_S,
    HILNAV_BARO_IIR_COEFF,
    HILNAV_BARO_ODR_HZ, HILNAV_BARO_SIGMA_PA, HILNAV_BARO_LSB_PA, HILNAV_BARO_MODEL_SIGMA_M,
    HILNAV_USE_BARO_LAG_MODEL,
    HILNAV_BARO_GATE_NIS,
    HILNAV_USE_ACCEL,
    HILNAV_ACCEL_SIGMA_MPS2, HILNAV_ACCEL_TILT_SIGMA_RAD, HILNAV_ACCEL_GATE_NIS,
    HILNAV_ACCEL_REACQUIRE_AFTER,
    HILNAV_ACCEL_OFFSET_MPS2,
    HILNAV_USE_IMU_TILT, HILNAV_USE_STATIONARY,
    HILNAV_STATIONARY_RATE_RAD_S, HILNAV_STATIONARY_ACCEL_MPS2, HILNAV_STATIONARY_HOLD_S,
    HILNAV_STATIONARY_MAX_RANGE_M,
    HILNAV_ZERO_VELOCITY_SIGMA, HILNAV_OFFSET_LEARN_TAU_S,
    HILNAV_REACQUIRE_AFTER_REJECTS,
    HILNAV_INIT_VEL_SIGMA, HILNAV_INIT_ACC_SIGMA, HILNAV_INIT_ACCEL
};

/* The barometer's two lags: the larger is the state, the smaller a first-order term in its measurement row. */
static void hn_derive_lag(HilNav* Nav) {
    const double vent = hn_max(1.0e-6, hn_cfg.BaroTauVentS);
    double iir = 0.0;
    const double dt = (hn_cfg.BaroOdrHz > 0.0) ? 1.0 / hn_cfg.BaroOdrHz : 0.0;
    if (hn_cfg.BaroIirCoeff > 0 && dt > 0.0) {
        const double pole = (double)hn_cfg.BaroIirCoeff / ((double)hn_cfg.BaroIirCoeff + 1.0);
        iir = -dt / log(pole);
    }
    Nav->TauVent = hn_max(vent, iir);
    Nav->TauIir = hn_min(vent, iir);
}

static void hn_baro_row(const HilNav* Nav, double* H) {
    int i;
    for (i = 0; i < HN_N; ++i) H[i] = 0.0;
    H[HILNAV_B] = 1.0;
    if (!hn_cfg.UseBaroLagModel) { H[HILNAV_H] = 1.0; return; }
    {
        const double k = Nav->TauIir / Nav->TauVent;
        H[HILNAV_H] = -k;
        H[HILNAV_M] = 1.0 + k;
    }
}

int HilNavInit(HilNav* Nav) {
    unsigned char* bytes;
    unsigned long k;
    if (Nav == 0) return 0;
    bytes = (unsigned char*)Nav;
    for (k = 0; k < (unsigned long)sizeof(HilNav); ++k) bytes[k] = 0;
    hn_derive_lag(Nav);
    HilNavReset(Nav);
    return HILNAV_READOUT;
}

void HilNavReset(HilNav* Nav) {
    hn_kf_reset(Nav, 0, 0);
    Nav->T = 0.0;
    Nav->Armed = 0;
    Nav->CosTilt = 1.0;
    Nav->LearnedOffset = hn_cfg.AccelOffsetMps2;
    Nav->Stationary = 0;
    Nav->HaveImu = 0;
    Nav->WinHead = 0;
    Nav->WinCount = 0;
    Nav->QuietSinceT = 0.0;
    Nav->LastImuT = 0.0;
    Nav->LastRangeT = -1.0;
    Nav->LastRangeValid = 0;
    Nav->NStationary = 0;
    Nav->LidarStatus = Nav->BaroStatus = Nav->AccelStatus = HILNAV_NO_SAMPLE;
    Nav->LidarZ = Nav->LidarInnov = Nav->LidarNis = 0.0;
    Nav->BaroZ = Nav->BaroInnov = Nav->BaroNis = 0.0;
    Nav->AccelZ = Nav->AccelInnov = Nav->AccelNis = 0.0;
    Nav->LastRange = 0.0;
    Nav->NLidarOk = Nav->NLidarBad = Nav->NBaroOk = Nav->NBaroBad = Nav->NAccelOk = Nav->NAccelBad = 0;
    Nav->LidarRejectRun = Nav->BaroRejectRun = Nav->AccelRejectRun = 0;
    Nav->NLidarReacq = Nav->NBaroReacq = Nav->NAccelReacq = 0;
}

void HilNavResetTo(HilNav* Nav, double H0, double V0, double SigmaH) {
    double x[HN_N], p[HN_N];
    HilNavReset(Nav);
    x[0] = H0;
    x[1] = V0;
    x[2] = hn_cfg.InitAccel;
    x[3] = H0 - Nav->TauVent * V0;
    x[4] = 0.0;
    p[0] = hn_sq(SigmaH);
    p[1] = hn_sq(hn_cfg.InitVelSigma);
    p[2] = hn_sq(hn_cfg.InitAccSigma);
    p[3] = hn_sq(SigmaH) + hn_sq(Nav->TauVent * hn_cfg.InitVelSigma);
    p[4] = hn_sq(hn_cfg.BiasSigmaM);
    hn_kf_reset(Nav, x, p);
    Nav->Armed = 1;
}

void HilNavSetCosTilt(HilNav* Nav, double CosTilt) {
    Nav->CosTilt = hn_min(1.0, hn_max(0.05, CosTilt));
}

/* Arm from whichever part reported first. From the barometer, h and b start correlated: it measures their sum. */
static void hn_arm(HilNav* Nav, double Z, double VarZ, int BiasCorrelated) {
    const double vb = hn_sq(hn_cfg.BiasSigmaM);
    double x[HN_N], p[HN_N];
    x[0] = Z;
    x[1] = 0.0;
    x[2] = hn_cfg.InitAccel;
    x[3] = Z;
    x[4] = 0.0;
    p[0] = BiasCorrelated ? (VarZ + vb) : VarZ;
    p[1] = hn_sq(hn_cfg.InitVelSigma);
    p[2] = hn_sq(hn_cfg.InitAccSigma);
    p[3] = 0.0;
    p[4] = vb;
    hn_kf_reset(Nav, x, p);
    if (BiasCorrelated) {
        hn_set_cov(Nav, HILNAV_M, HILNAV_M, VarZ + vb);
        hn_set_cov(Nav, HILNAV_H, HILNAV_M, VarZ + vb);
        hn_set_cov(Nav, HILNAV_H, HILNAV_B, -vb);
        hn_set_cov(Nav, HILNAV_M, HILNAV_B, -vb);
    } else {
        hn_set_cov(Nav, HILNAV_M, HILNAV_M, VarZ + hn_sq(Nav->TauVent * hn_cfg.InitVelSigma));
        hn_set_cov(Nav, HILNAV_H, HILNAV_M, VarZ);
    }
    Nav->Armed = 1;
}

/* Exact discretisation: the triple integrator, the enclosure's closed-form row, the bias's decay. */
void HilNavPredictTo(HilNav* Nav, double T) {
    double F[HN_N * HN_N], Q[HN_N * HN_N];
    double dt, tv, gone, E, decay, q, d2, d3, d4, d5;
    int i;
    if (!Nav->Armed) { Nav->T = T; return; }
    dt = T - Nav->T;
    if (!(dt > 0.0)) { Nav->T = hn_max(Nav->T, T); return; }

    tv = Nav->TauVent;
    gone = -expm1(-dt / tv);
    E = 1.0 - gone;

    for (i = 0; i < HN_N * HN_N; ++i) F[i] = 0.0;
    for (i = 0; i < HN_N; ++i) F[i * HN_N + i] = 1.0;
    F[HILNAV_H * HN_N + HILNAV_V] = dt;
    F[HILNAV_H * HN_N + HILNAV_A] = 0.5 * dt * dt;
    F[HILNAV_V * HN_N + HILNAV_A] = dt;

    F[HILNAV_M * HN_N + HILNAV_H] = gone;
    F[HILNAV_M * HN_N + HILNAV_V] = dt - tv * gone;
    F[HILNAV_M * HN_N + HILNAV_A] = 0.5 * dt * dt - tv * dt + tv * tv * gone;
    F[HILNAV_M * HN_N + HILNAV_M] = E;

    decay = (hn_cfg.BiasTauS > 0.0) ? exp(-dt / hn_cfg.BiasTauS) : 1.0;
    F[HILNAV_B * HN_N + HILNAV_B] = decay;

    q = hn_max(0.0, hn_cfg.JerkPsd);
    d2 = dt * dt; d3 = d2 * dt; d4 = d3 * dt; d5 = d4 * dt;
    for (i = 0; i < HN_N * HN_N; ++i) Q[i] = 0.0;
    Q[HILNAV_H * HN_N + HILNAV_H] = q * d5 / 20.0;
    Q[HILNAV_H * HN_N + HILNAV_V] = Q[HILNAV_V * HN_N + HILNAV_H] = q * d4 / 8.0;
    Q[HILNAV_H * HN_N + HILNAV_A] = Q[HILNAV_A * HN_N + HILNAV_H] = q * d3 / 6.0;
    Q[HILNAV_V * HN_N + HILNAV_V] = q * d3 / 3.0;
    Q[HILNAV_V * HN_N + HILNAV_A] = Q[HILNAV_A * HN_N + HILNAV_V] = q * d2 / 2.0;
    Q[HILNAV_A * HN_N + HILNAV_A] = q * dt;
    Q[HILNAV_B * HN_N + HILNAV_B] =
        (hn_cfg.BiasTauS > 0.0) ? hn_sq(hn_cfg.BiasSigmaM) * (1.0 - decay * decay) : 0.0;

    hn_kf_predict(Nav, F, Q);
    Nav->T = T;
}

/* After enough refusals in a row the filter, not the part, is wrong: open the gate by the disagreement. */
static int hn_reacquire(HilNav* Nav, int* Run, int After, int StateIdx, double Innovation) {
    double want;
    if (After <= 0 || *Run < After) return 0;
    want = Innovation * Innovation;
    if (Nav->P[StateIdx * HN_N + StateIdx] < want) hn_set_cov(Nav, StateIdx, StateIdx, want);
    *Run = 0;
    return 1;
}

int HilNavPushRange(HilNav* Nav, double RangeM, double Strength, double TSample, int Valid) {
    hn_outcome o;
    double rc, arm, z, thr, weak, sr, sinT, st, R;
    Nav->LastRange = RangeM;
    Nav->LastRangeT = TSample;
    Nav->LastRangeValid = Valid ? 1 : 0;
    if (!hn_cfg.UseLidar) { Nav->LidarStatus = HILNAV_NO_SAMPLE; return Nav->LidarStatus; }

    if (!Valid || RangeM < hn_cfg.LidarRangeMinM || RangeM > hn_cfg.LidarRangeMaxM) {
        Nav->LidarStatus = HILNAV_UNUSABLE;
        ++Nav->NLidarBad;
        return Nav->LidarStatus;
    }
    if (hn_cfg.LidarMaxTiltRad > 0.0 && Nav->CosTilt < cos(hn_cfg.LidarMaxTiltRad)) {
        Nav->LidarStatus = HILNAV_UNUSABLE;
        ++Nav->NLidarBad;
        return Nav->LidarStatus;
    }

    HilNavPredictTo(Nav, TSample);

    rc = RangeM + 0.5 * hn_cfg.LidarLsbM;
    arm = rc + hn_cfg.MountBelowCgM;
    z = arm * Nav->CosTilt;
    Nav->LidarZ = z;

    thr = hn_max(1.0, (double)hn_cfg.LidarStrengthThreshold);
    weak = sqrt(hn_max(1.0, 4.0 * thr / hn_max(Strength, 1.0)));
    sr = hn_cfg.LidarSigmaM * weak;
    sinT = sqrt(hn_max(0.0, 1.0 - Nav->CosTilt * Nav->CosTilt));
    st = hn_max(0.0, hn_cfg.TiltSigmaRad);
    R = hn_sq(sr) + hn_sq(hn_cfg.LidarLsbM) / 12.0
      + hn_sq(arm * sinT * st) + hn_sq(0.5 * arm * st * st);

    if (!Nav->Armed) {
        hn_arm(Nav, z, R, 0);
        Nav->LidarStatus = HILNAV_ACCEPTED;
        Nav->LidarInnov = 0.0;
        Nav->LidarNis = 0.0;
        ++Nav->NLidarOk;
        return Nav->LidarStatus;
    }

    o = hn_kf_update(Nav, hn_HLidar, z, R, hn_cfg.LidarGateNis);
    if (!o.Accepted) {
        ++Nav->LidarRejectRun;
        if (hn_reacquire(Nav, &Nav->LidarRejectRun, hn_cfg.ReacquireAfterRejects, HILNAV_H, o.Innovation)) {
            ++Nav->NLidarReacq;
            o = hn_kf_update(Nav, hn_HLidar, z, R, hn_cfg.LidarGateNis);
        }
    }
    Nav->LidarInnov = o.Innovation;
    Nav->LidarNis = o.Nis;
    if (o.Accepted) { Nav->LidarStatus = HILNAV_ACCEPTED; ++Nav->NLidarOk; Nav->LidarRejectRun = 0; }
    else            { Nav->LidarStatus = HILNAV_GATED;    ++Nav->NLidarBad; }
    return Nav->LidarStatus;
}

int HilNavPushBaro(HilNav* Nav, double AltitudeM, double PressurePa, double TSample, int Valid) {
    hn_outcome o;
    double H[HN_N];
    double p, grad, c, sigPa, lsb, R;
    if (!hn_cfg.UseBaro) { Nav->BaroStatus = HILNAV_NO_SAMPLE; return Nav->BaroStatus; }
    if (!Valid) {
        Nav->BaroStatus = HILNAV_UNUSABLE;
        ++Nav->NBaroBad;
        return Nav->BaroStatus;
    }

    HilNavPredictTo(Nav, TSample);
    Nav->BaroZ = AltitudeM;

    p = hn_max(1000.0, PressurePa);
    grad = fabs((hn_isa_altitude(p + 50.0) - hn_isa_altitude(p - 50.0)) / 100.0);
    c = (double)((0 < hn_cfg.BaroIirCoeff) ? hn_cfg.BaroIirCoeff : 0);
    sigPa = hn_cfg.BaroSigmaPa / sqrt(2.0 * c + 1.0);
    lsb = hn_cfg.BaroLsbPa * grad;
    R = hn_sq(sigPa * grad) + hn_sq(lsb) / 12.0 + hn_sq(hn_max(0.0, hn_cfg.BaroModelSigmaM));

    if (!Nav->Armed) {
        hn_arm(Nav, AltitudeM, R, 1);
        Nav->BaroStatus = HILNAV_ACCEPTED;
        Nav->BaroInnov = 0.0;
        Nav->BaroNis = 0.0;
        ++Nav->NBaroOk;
        return Nav->BaroStatus;
    }

    hn_baro_row(Nav, H);
    o = hn_kf_update(Nav, H, AltitudeM, R, hn_cfg.BaroGateNis);
    if (!o.Accepted) {
        ++Nav->BaroRejectRun;
        if (hn_reacquire(Nav, &Nav->BaroRejectRun, hn_cfg.ReacquireAfterRejects, HILNAV_B, o.Innovation)) {
            ++Nav->NBaroReacq;
            o = hn_kf_update(Nav, H, AltitudeM, R, hn_cfg.BaroGateNis);
        }
    }
    Nav->BaroInnov = o.Innovation;
    Nav->BaroNis = o.Nis;
    if (o.Accepted) { Nav->BaroStatus = HILNAV_ACCEPTED; ++Nav->NBaroOk; Nav->BaroRejectRun = 0; }
    else            { Nav->BaroStatus = HILNAV_GATED;    ++Nav->NBaroBad; }
    return Nav->BaroStatus;
}

int HilNavPushAccel(HilNav* Nav, double AccelUpMps2, double HorizAccelMps2, double TSample, int Valid) {
    hn_outcome o;
    double up, se, R;
    if (!hn_cfg.UseAccel) { Nav->AccelStatus = HILNAV_NO_SAMPLE; return Nav->AccelStatus; }
    if (!Valid) {
        Nav->AccelStatus = HILNAV_UNUSABLE;
        ++Nav->NAccelBad;
        return Nav->AccelStatus;
    }
    if (!Nav->Armed) { Nav->AccelStatus = HILNAV_NO_SAMPLE; return Nav->AccelStatus; }

    HilNavPredictTo(Nav, TSample);
    up = AccelUpMps2 - Nav->LearnedOffset;
    Nav->AccelZ = up;

    se = hn_max(0.0, hn_cfg.AccelTiltSigmaRad);
    R = hn_sq(hn_max(0.0, hn_cfg.AccelSigmaMps2)) + hn_sq(se * fabs(HorizAccelMps2));

    o = hn_kf_update(Nav, hn_HAccel, up, R, hn_cfg.AccelGateNis);
    if (!o.Accepted) {
        ++Nav->AccelRejectRun;
        if (hn_reacquire(Nav, &Nav->AccelRejectRun, hn_cfg.AccelReacquireAfter, HILNAV_A, o.Innovation)) {
            ++Nav->NAccelReacq;
            o = hn_kf_update(Nav, hn_HAccel, up, R, hn_cfg.AccelGateNis);
        }
    }
    Nav->AccelInnov = o.Innovation;
    Nav->AccelNis = o.Nis;
    if (o.Accepted) { Nav->AccelStatus = HILNAV_ACCEPTED; ++Nav->NAccelOk; Nav->AccelRejectRun = 0; }
    else            { Nav->AccelStatus = HILNAV_GATED;    ++Nav->NAccelBad; }
    return Nav->AccelStatus;
}

/* Standing on the ground: IMU at rest over a window AND the beam on the ground. Pins v, learns the offset. */
void HilNavPushStationaryEvidence(HilNav* Nav, double AccelUpMps2, double HorizAccelMps2, double RateRadS,
                                  double TSample) {
    double up, f, w, dt, window, sf, sf2, sw, meanF, sdF, meanW;
    int n, i, quiet, onGround, wasStationary;
    if (!hn_cfg.UseStationary) { Nav->Stationary = 0; return; }

    up = AccelUpMps2 + hn_g;
    f = sqrt(HorizAccelMps2 * HorizAccelMps2 + up * up);
    w = fabs(RateRadS);

    dt = Nav->HaveImu ? hn_max(0.0, TSample - Nav->LastImuT) : 0.0;
    if (!Nav->HaveImu) { Nav->QuietSinceT = TSample; Nav->HaveImu = 1; }
    Nav->LastImuT = TSample;

    window = hn_max(1.0e-3, 0.5 * hn_cfg.StationaryHoldS);
    Nav->Win[Nav->WinHead].T = TSample;
    Nav->Win[Nav->WinHead].F = f;
    Nav->Win[Nav->WinHead].W = w;
    Nav->WinHead = (Nav->WinHead + 1) % HILNAV_WINDOW;
    if (Nav->WinCount < HILNAV_WINDOW) ++Nav->WinCount;

    n = 0;
    sf = 0.0; sf2 = 0.0; sw = 0.0;
    for (i = 0; i < Nav->WinCount; ++i) {
        const HilNavImuSample* smp = &Nav->Win[(Nav->WinHead - 1 - i + HILNAV_WINDOW) % HILNAV_WINDOW];
        if (TSample - smp->T > window) break;
        sf += smp->F; sf2 += smp->F * smp->F; sw += smp->W; ++n;
    }
    meanF = (n > 0) ? sf / n : 0.0;
    sdF = (n > 1) ? sqrt(hn_max(0.0, sf2 / n - meanF * meanF)) : 1.0e9;
    meanW = (n > 0) ? sw / n : 1.0e9;

    quiet = n >= 5 && meanW < hn_cfg.StationaryRateRadS && sdF < hn_cfg.StationaryAccelMps2 &&
            fabs(meanF - hn_g) < 1.0;
    if (!quiet) Nav->QuietSinceT = TSample;

    onGround = Nav->LastRangeValid && Nav->LastRangeT >= 0.0 && (TSample - Nav->LastRangeT) < 0.25 &&
               Nav->LastRange < hn_cfg.StationaryMaxRangeM;
    wasStationary = Nav->Stationary;
    Nav->Stationary = quiet && onGround && (TSample - Nav->QuietSinceT) >= hn_cfg.StationaryHoldS;
    /* Leaving the pad hands the acceleration back its power-up uncertainty. */
    if (wasStationary && !Nav->Stationary && Nav->Armed &&
        Nav->P[HILNAV_A * HN_N + HILNAV_A] < hn_sq(hn_cfg.InitAccSigma))
        hn_set_cov(Nav, HILNAV_A, HILNAV_A, hn_sq(hn_cfg.InitAccSigma));
    if (!Nav->Stationary) return;

    ++Nav->NStationary;
    if (dt > 0.0)
        Nav->LearnedOffset += -expm1(-dt / hn_max(1.0e-3, hn_cfg.OffsetLearnTauS))
                            * (AccelUpMps2 - Nav->LearnedOffset);

    if (!Nav->Armed) return;
    HilNavPredictTo(Nav, TSample);
    hn_kf_update(Nav, hn_HVel, 0.0, hn_sq(hn_max(1.0e-4, hn_cfg.ZeroVelocitySigma)), 0.0);
}

/* q = (w, x, y, z), body -> world. R33 is the beam's cosine; the rotated specific force less g is the
 * acceleration the filter measures, and its horizontal part prices the attitude error. */
void HilNavVerticalFromQuat(const double Q[4], const double AccBody[3],
                            double* OutCosTilt, double* OutAccelUp, double* OutAccelHoriz) {
    const double w = Q[0], x = Q[1], y = Q[2], z = Q[3];
    const double r11 = 1.0 - 2.0 * (y * y + z * z), r12 = 2.0 * (x * y - w * z), r13 = 2.0 * (x * z + w * y);
    const double r21 = 2.0 * (x * y + w * z), r22 = 1.0 - 2.0 * (x * x + z * z), r23 = 2.0 * (y * z - w * x);
    const double r31 = 2.0 * (x * z - w * y), r32 = 2.0 * (y * z + w * x);
    const double r33 = 1.0 - 2.0 * (x * x + y * y);

    const double fx = r11 * AccBody[0] + r12 * AccBody[1] + r13 * AccBody[2];
    const double fy = r21 * AccBody[0] + r22 * AccBody[1] + r23 * AccBody[2];
    const double fz = r31 * AccBody[0] + r32 * AccBody[1] + r33 * AccBody[2];

    *OutCosTilt = r33;
    *OutAccelUp = fz - hn_g;
    *OutAccelHoriz = sqrt(fx * fx + fy * fy);
}

/* One whole IMU sample, in the order the simulator's own update uses: tilt, standing still, then the
 * accelerometer as a measurement. */
int HilNavPushImu(HilNav* Nav, const double QuatWxyz[4], const double AccelBody[3], const double RatesBody[3],
                  double TSample, int Valid) {
    double ct = 1.0, up = 0.0, horiz = 0.0;
    HilNavVerticalFromQuat(QuatWxyz, AccelBody, &ct, &up, &horiz);
    if (hn_cfg.UseImuTilt && Valid) HilNavSetCosTilt(Nav, ct);
    if (RatesBody != 0 && Valid) {
        const double rate = sqrt(RatesBody[0] * RatesBody[0] + RatesBody[1] * RatesBody[1] +
                                 RatesBody[2] * RatesBody[2]);
        HilNavPushStationaryEvidence(Nav, up, horiz, rate, TSample);
    }
    return HilNavPushAccel(Nav, up, horiz, TSample, Valid);
}

int HilNavRead(const HilNav* Nav, double* Out, int Cap) {
    double v[HILNAV_READOUT];
    int i, n;
    if (Nav == 0 || Out == 0 || Cap <= 0) return 0;
    v[0] = Nav->T;
    v[1] = Nav->Armed ? 1.0 : 0.0;
    for (i = 0; i < HN_N; ++i) v[2 + i] = Nav->X[i];
    for (i = 0; i < HN_N; ++i) v[7 + i] = hn_sigma(Nav, i);
    v[12] = Nav->LidarZ; v[13] = Nav->LidarInnov; v[14] = Nav->LidarNis; v[15] = (double)Nav->LidarStatus;
    v[16] = Nav->BaroZ;  v[17] = Nav->BaroInnov;  v[18] = Nav->BaroNis;  v[19] = (double)Nav->BaroStatus;
    v[20] = (double)Nav->NLidarOk; v[21] = (double)Nav->NLidarBad;
    v[22] = (double)Nav->NBaroOk;  v[23] = (double)Nav->NBaroBad;
    v[24] = (double)Nav->NLidarReacq; v[25] = (double)Nav->NBaroReacq;
    v[26] = Nav->AccelZ; v[27] = Nav->AccelInnov; v[28] = Nav->AccelNis; v[29] = (double)Nav->AccelStatus;
    v[30] = (double)Nav->NAccelOk; v[31] = (double)Nav->NAccelBad; v[32] = (double)Nav->NAccelReacq;
    v[33] = Nav->LearnedOffset;
    v[34] = Nav->Stationary ? 1.0 : 0.0;
    n = (Cap < HILNAV_READOUT) ? Cap : HILNAV_READOUT;
    for (i = 0; i < n; ++i) Out[i] = v[i];
    return n;
}

const char* HilNavReadoutName(int Index) {
    static const char* const kNames[HILNAV_READOUT] = {
        "t", "armed", "h", "v", "a", "baro_lag", "bias",
        "sigma_h", "sigma_v", "sigma_a", "sigma_lag", "sigma_bias",
        "lidar_z", "lidar_innov", "lidar_nis", "lidar_status",
        "baro_z", "baro_innov", "baro_nis", "baro_status",
        "lidar_ok", "lidar_bad", "baro_ok", "baro_bad",
        "lidar_reacq", "baro_reacq",
        "accel_z", "accel_innov", "accel_nis", "accel_status",
        "accel_ok", "accel_bad", "accel_reacq",
        "accel_offset", "stationary"
    };
    return (Index >= 0 && Index < HILNAV_READOUT) ? kNames[Index] : "";
}
