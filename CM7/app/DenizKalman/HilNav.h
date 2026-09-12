#ifndef HIL_NAV_H
#define HIL_NAV_H

/* HilNav.h -- THE ALTITUDE FILTER, AS PLAIN C FOR THE FLIGHT COMPUTER.
 *
 * The same five-state Kalman filter the simulator runs (`components/estimation/AltitudeEstimator`), written in
 * the common subset of C99 and C++: it compiles as either, includes only <math.h>, allocates nothing, reads no
 * file and keeps no globals. The caller owns one `HilNav` per filter.
 *
 * CONFIGURED AT COMPILE TIME, like the static plant. Every setting is a constant in `HilNavConfig.h` -- the
 * vehicle's parts, the tuning fitted to its logs, where the rangefinder is mounted and which point's altitude is
 * reported (the CG). Changing any of it is an edit to that header and a rebuild; there is no configuration call.
 *
 * IT IS A COPY, AND A TEST KEEPS IT HONEST. `tests/components/estimation/HilNavMatchesEstimatorTests.cpp` feeds
 * this and the simulator's filter the same samples, configured from that header, and requires every output to
 * match BIT FOR BIT, so a change to one is a change to both. The reasoning behind every term is written out in
 * AltitudeEstimator.h/.cpp.
 *
 * WHAT IT ESTIMATES -- the vertical channel, x = [ h, v, a, m, b ]:
 *     h  altitude of the CG above the ground [m]        m  the barometer's enclosure altitude [m]
 *     v  vertical velocity [m/s]                         b  the barometer's offset [m]
 *     a  vertical acceleration [m/s^2]
 * Attitude comes from the IMU's own solution. Nothing here estimates horizontal position.
 *
 * ON THE DEVICE:
 *
 *     static HilNav nav;
 *     HilNavInit(&nav);
 *     every loop, pushing each part ONLY when it has a new sample, IMU first:
 *         HilNavPushImu(&nav, quat_wxyz, accel_body, gyro_body, t_imu, 1);
 *         HilNavPushRange(&nav, range_mm / 1000.0, strength, t_range, range_valid);
 *         HilNavPushBaro(&nav, baro_altitude_m, baro_pressure_pa, t_baro, 1);
 *         HilNavPredictTo(&nav, t_now);
 *         HilNavRead(&nav, out, HILNAV_READOUT);       out[2] altitude, out[3] vertical speed, ...
 *
 * WHAT THE CALLER MUST DO:
 *  - Push a sample ONCE. A driver republishing its last reading between samples must not be fed it again: a
 *    repeat counts as new information and shrinks the covariance to a confidence the part never earned.
 *  - Timestamps in seconds on one clock. Push samples oldest first; the filter will not predict backwards.
 *  - Push the IMU before a rangefinder sample of the same instant: it sets the tilt the range is projected with.
 *  - If the attitude updates more often than the accelerometer, hand each new quaternion to
 *    HilNavVerticalFromQuat and HilNavSetCosTilt, as the log replay does.
 *  - SI units throughout: metres, seconds, pascals, m/s^2, rad/s.
 */

#ifdef __cplusplus
extern "C" {
#endif

enum {
    HILNAV_STATES  = 5,
    HILNAV_READOUT = 35,     /* doubles HilNavRead writes */
    HILNAV_WINDOW  = 128     /* IMU samples the standing-still detector can hold */
};

/* State indices, and what happened to a channel's latest sample. */
enum { HILNAV_H = 0, HILNAV_V = 1, HILNAV_A = 2, HILNAV_M = 3, HILNAV_B = 4 };
enum { HILNAV_NO_SAMPLE = 0, HILNAV_ACCEPTED = 1, HILNAV_GATED = 2, HILNAV_UNUSABLE = 3 };

typedef struct HilNavImuSample { double T, F, W; } HilNavImuSample;

/* THE WHOLE FILTER, as plain data with no pointers inside: it can be copied, zeroed or kept in static memory
 * (about 3.5 kB). Read it through HilNavRead; the fields are its working storage. */
typedef struct HilNav {
    double X[HILNAV_STATES];
    double P[HILNAV_STATES * HILNAV_STATES];
    double T;
    int    Armed;
    double CosTilt, TauVent, TauIir;
    int    LidarStatus, BaroStatus, AccelStatus;
    double LidarZ, LidarInnov, LidarNis, BaroZ, BaroInnov, BaroNis, AccelZ, AccelInnov, AccelNis;
    double LastRange, LastRangeT;
    int    LastRangeValid, HaveImu, Stationary;
    HilNavImuSample Win[HILNAV_WINDOW];
    int    WinHead, WinCount;
    double QuietSinceT, LastImuT, LearnedOffset;
    long   NStationary;
    int    NLidarOk, NLidarBad, NBaroOk, NBaroBad, NAccelOk, NAccelBad;
    int    LidarRejectRun, BaroRejectRun, AccelRejectRun;
    int    NLidarReacq, NBaroReacq, NAccelReacq;
} HilNav;

/* Setup. HilNavInit leaves the filter DISARMED -- it arms on the first usable rangefinder or barometer sample --
 * and returns HILNAV_READOUT. HilNavReset disarms again; HilNavResetTo arms at a known altitude. */
int  HilNavInit(HilNav* Nav);
void HilNavReset(HilNav* Nav);
void HilNavResetTo(HilNav* Nav, double H0, double V0, double SigmaH);

/* Driving it. Each push returns that channel's HILNAV_* status. Valid is the part's own verdict, faults
 * included: 0 refuses the sample without touching the state. */
void HilNavPredictTo(HilNav* Nav, double T);
int  HilNavPushRange(HilNav* Nav, double RangeM, double Strength, double TSample, int Valid);
int  HilNavPushBaro(HilNav* Nav, double AltitudeM, double PressurePa, double TSample, int Valid);
int  HilNavPushImu(HilNav* Nav, const double QuatWxyz[4], const double AccelBody[3], const double RatesBody[3],
                   double TSample, int Valid);

/* The IMU push in pieces, for a caller with its own attitude plumbing. RatesBody may be 0 in HilNavPushImu,
 * which then never judges the vehicle to be standing still. */
void HilNavVerticalFromQuat(const double QuatWxyz[4], const double AccelBody[3],
                            double* CosTilt, double* AccelUpMps2, double* HorizAccelMps2);
void HilNavSetCosTilt(HilNav* Nav, double CosTilt);
void HilNavPushStationaryEvidence(HilNav* Nav, double AccelUpMps2, double HorizAccelMps2, double RateRadS,
                                  double TSample);
int  HilNavPushAccel(HilNav* Nav, double AccelUpMps2, double HorizAccelMps2, double TSample, int Valid);

/* Reading it: the same 35 values, in the same order, as AltitudeEstimator::Readout. */
int         HilNavRead(const HilNav* Nav, double* Out, int Cap);
const char* HilNavReadoutName(int Index);

#ifdef __cplusplus
}
#endif

#endif /* HIL_NAV_H */
