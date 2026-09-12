#ifndef HIL_NAV_CONFIG_H
#define HIL_NAV_CONFIG_H

/* HilNavConfig.h -- THE ALTITUDE FILTER'S SETTINGS, AS COMPILE-TIME CONSTANTS.
 *
 * The configuration for `HilNav.cpp`, in the same way `HilStaticConfig.h` is for the static plant: nothing is
 * read at run time -- no file, no parser, no configuration call. Re-tuning the filter is an edit to this header
 * and a rebuild.
 *
 * THE NUMBERS ARE THE FLIGHT PROFILE: the vehicle's own parts -- the TF02-Pro in millimetres at 1 kHz, a vented
 * BMP581 at 100 Hz, the BNO085 -- and the tuning fitted to its three flight-computer logs in Plan 053. They are
 * `AltitudeEstimator::Config::ForFlightLog()` value for value, and `HilNavMatchesEstimatorTests` checks that, so
 * this header and the simulator's filter cannot drift apart unnoticed. The reasoning behind each value is written
 * beside it there.
 *
 * MACROS, NOT TYPED CONSTANTS, because this header must compile as C, and in C only a macro is a constant
 * expression. UNITS: metres, seconds, radians, pascals. A `USE_` switch is 1 for on, 0 for off.
 */

/* ---- which point's altitude, and where the rangefinder is ----------------------------------------------- */
/* Heights along the body z axis, measured from the body origin, positive up.
 *
 * THE POINT REPORTED is the CG -- the point the static plant's position state describes, so the filter and the
 * plant talk about the same thing. It is HilStaticConfig.h's kCgZ, and the test pins the two together. Set it
 * to 0.0 to report the body origin instead. */
#define HILNAV_REF_Z                      (-0.25830)
/* THE TF02-PRO'S MOUNT: 0.85 m below the body origin, the value the simulated vehicle carries. Measure it on the
 * airframe and write the real one here: every centimetre of error is a centimetre of altitude. */
#define HILNAV_LIDAR_MOUNT_Z              (-0.85)

/* ---- process ------------------------------------------------------------------------------------------- */
#define HILNAV_JERK_PSD                   60.0          /* white jerk [m^2/s^5]; fitted to a hand test -- refit
                                                           from the first real flight */
#define HILNAV_BIAS_SIGMA_M               1.5           /* the barometer's offset, 1 sigma [m] */
#define HILNAV_BIAS_TAU_S                 1800.0        /* how fast that offset may wander [s] */

/* ---- the rangefinder, TF02-Pro -------------------------------------------------------------------------- */
#define HILNAV_USE_LIDAR                  1
#define HILNAV_LIDAR_SIGMA_M              0.058         /* sized by NIS on the logs */
#define HILNAV_LIDAR_STRENGTH_THRESHOLD   60            /* the part's own detection threshold */
#define HILNAV_LIDAR_LSB_M                0.001         /* millimetre output */
#define HILNAV_LIDAR_RANGE_MIN_M          0.02          /* the logs read valid ranges down to 3.3 cm */
#define HILNAV_LIDAR_RANGE_MAX_M          40.0
#define HILNAV_TILT_SIGMA_RAD             0.0349065850398866   /* 2 degrees: the attitude solution's error */
#define HILNAV_LIDAR_MAX_TILT_RAD         0.7853981633974483   /* 45 degrees: past it the beam is refused */
#define HILNAV_LIDAR_GATE_NIS             25.0          /* five sigma */

/* ---- the barometer, BMP581 ------------------------------------------------------------------------------ */
#define HILNAV_USE_BARO                   1
#define HILNAV_BARO_TAU_VENT_S            0.015         /* vented: the logs bound the whole lag at 0-30 ms */
#define HILNAV_BARO_IIR_COEFF             0             /* the on-chip IIR, bypassed */
#define HILNAV_BARO_ODR_HZ                100.0
#define HILNAV_BARO_SIGMA_PA              0.21          /* datasheet, OSR 16 */
#define HILNAV_BARO_LSB_PA                (1.0 / 64.0)
#define HILNAV_BARO_MODEL_SIGMA_M         0.12          /* the room's air, measured two ways on the logs */
#define HILNAV_USE_BARO_LAG_MODEL         1
#define HILNAV_BARO_GATE_NIS              25.0

/* ---- the accelerometer, BNO085 -------------------------------------------------------------------------- */
#define HILNAV_USE_ACCEL                  1
#define HILNAV_ACCEL_SIGMA_MPS2           0.05
#define HILNAV_ACCEL_TILT_SIGMA_RAD       0.0174532925199433   /* 1 degree, charged to sideways force */
#define HILNAV_ACCEL_GATE_NIS             25.0
#define HILNAV_ACCEL_REACQUIRE_AFTER      2             /* the gate gives up after two refusals: a valve opening
                                                           is not an outlier */
#define HILNAV_ACCEL_OFFSET_MPS2          0.0           /* the starting offset; standing still learns it */

/* ---- attitude, and standing still ----------------------------------------------------------------------- */
#define HILNAV_USE_IMU_TILT               1             /* project the beam with the IMU's attitude */
#define HILNAV_USE_STATIONARY             1
#define HILNAV_STATIONARY_RATE_RAD_S      0.15          /* mean |omega| below this ... */
#define HILNAV_STATIONARY_ACCEL_MPS2      0.2           /* ... |f| steady to this ... */
#define HILNAV_STATIONARY_HOLD_S          0.5           /* ... for this long ... */
#define HILNAV_STATIONARY_MAX_RANGE_M     0.15          /* ... with the beam on the ground */
#define HILNAV_ZERO_VELOCITY_SIGMA        0.01          /* [m/s] */
#define HILNAV_OFFSET_LEARN_TAU_S         1.0

/* ---- giving up on the gate, and the start ---------------------------------------------------------------- */
#define HILNAV_REACQUIRE_AFTER_REJECTS    20            /* rangefinder and barometer */
#define HILNAV_INIT_VEL_SIGMA             5.0           /* [m/s] */
#define HILNAV_INIT_ACC_SIGMA             15.0          /* [m/s^2] */
#define HILNAV_INIT_ACCEL                 (-9.80665)    /* free fall until told otherwise */

#endif /* HIL_NAV_CONFIG_H */
