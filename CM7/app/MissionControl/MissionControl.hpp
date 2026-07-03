/*
 * MissionControl.hpp
 *
 *  Created on: Jul 3, 2026
 *      Author: karadeli
 */

#ifndef MISSIONCONTROL_MISSIONCONTROL_HPP_
#define MISSIONCONTROL_MISSIONCONTROL_HPP_

#include <cstdint>

// ============================================================================
// MissionControl
// ----------------------------------------------------------------------------
// Central authority for run-time operating modes of the flight controller.
//   - Logging enable/disable (hooks into sd_log_start / sd_log_stop)
//   - Actuator control mode (what quantity the actuator loop regulates)
//   - System control mode   (what the vehicle as a whole is doing)
//
// Design constraints (embedded, STM32H755):
//   - No heap, no exceptions, no RTTI.
//   - Callbacks are plain function pointers in fixed-size tables.
//   - Single-core ownership: all reads and writes happen on one core (CM7).
//     Not safe for cross-core access as-is.
// ============================================================================

namespace mc {

// ---------------------------------------------------------------------------
// Mode enums
// ---------------------------------------------------------------------------
enum class ActuatorMode : uint8_t {
    Disable = 0,  // outputs off / loops not running
    Duty,         // open-loop PWM duty
    Current,      // current loop
    Speed,        // velocity loop
    Position,     // position loop
    Pressure,     // chamber/tank pressure loop
    Thrust,       // thrust loop (outermost)
    _Count
};

enum class SystemMode : uint8_t {
    Idle = 0,     // safe/default state: power-up, standby, post-flight, bench/debug
    FuncTest,     // scripted functionality test
    XIL,          // in-the-loop testing (SIL or HWIL, per build/config)
    Drop,         // drop test flight profile
    Hover,        // hover flight profile
    _Count
};

enum class Result : uint8_t {
    Ok = 0,
    InvalidTransition,   // blocked by the transition matrix
    Locked,              // MissionControl is locked (flight-critical phase)
    Rejected,            // vetoed by the application's system-mode guard
    NoChange             // requested mode == current mode
};

// ---------------------------------------------------------------------------
// Callback types (plain function pointers — no std::function / heap)
// ---------------------------------------------------------------------------
using ActuatorModeCb = void (*)(ActuatorMode oldMode, ActuatorMode newMode);
using SystemModeCb   = void (*)(SystemMode   oldMode, SystemMode   newMode);
using LoggingCb      = void (*)(bool enabled);

// Application veto over system-mode transitions. Return false to reject.
// Not consulted when the target is Idle (Idle must always be reachable).
using SystemModeGuard = bool (*)(SystemMode from, SystemMode to);

class MissionControl {
public:
    MissionControl();

    // ---- Logging ----------------------------------------------------------
    Result enableLogging();
    Result disableLogging();
    bool   isLoggingEnabled() const { return logging_; }

    // ---- Actuator control mode -------------------------------------------
    Result setActuatorMode(ActuatorMode m);
    ActuatorMode actuatorMode() const { return actuator_; }

    // ---- System control mode ---------------------------------------------
    Result setSystemMode(SystemMode m);
    SystemMode systemMode() const { return system_; }

    // ---- Lock (flight-critical phases) -------------------------------------
    // While locked, actuator & system mode changes are rejected.
    // Logging changes remain allowed (you still want the SD log
    // controllable during flight).
    void lock()     { locked_ = true; }
    void unlock()   { locked_ = false; }
    bool isLocked() const { return locked_; }

    // ---- Callback registration (one per event; call once at init) ----------
    void onActuatorModeChange(ActuatorModeCb cb) { actuatorCb_ = cb; }
    void onSystemModeChange(SystemModeCb cb)     { systemCb_ = cb; }
    void onLoggingChange(LoggingCb cb)           { loggingCb_ = cb; }

    // Application veto over system-mode transitions (e.g. require SD ready
    // before entering test/flight modes). Skipped when target is Idle.
    void setSystemModeGuard(SystemModeGuard g)   { systemGuard_ = g; }

private:
    // Transition guard matrices. Row = from, Col = to. true = allowed.
    static bool systemTransitionAllowed(SystemMode from, SystemMode to);
    static bool actuatorTransitionAllowed(SystemMode sys, ActuatorMode from, ActuatorMode to);

    ActuatorMode actuator_;
    SystemMode   system_;
    bool         logging_;
    bool         locked_;

    ActuatorModeCb  actuatorCb_  = nullptr;
    SystemModeCb    systemCb_    = nullptr;
    LoggingCb       loggingCb_   = nullptr;
    SystemModeGuard systemGuard_ = nullptr;
};

// Optional convenience: single global instance (define in one .cpp).
extern MissionControl g_mission;

} // namespace mc

#endif /* MISSIONCONTROL_MISSIONCONTROL_HPP_ */
