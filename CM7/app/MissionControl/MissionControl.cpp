/*
 * MissionControl.cpp
 *
 *  Created on: Jul 3, 2026
 *      Author: karadeli
 */

#include "MissionControl.hpp"

namespace mc {

MissionControl g_mission;

// ---------------------------------------------------------------------------
// Transition matrices — EDIT THESE to match your test campaign rules.
// ---------------------------------------------------------------------------

// System mode transitions. Row = from, Col = to.
// Philosophy of the defaults:
//   - Idle is the safe hub AND the bench/debug state: power-up, standby,
//     post-flight, manual poking. Everything can fall back to Idle (abort).
//   - Flight profiles (Drop, Hover) are only reachable from FuncTest or XIL,
//     i.e. after the vehicle has been exercised — never straight from Idle.
//   - Drop <-> Hover direct switching is disallowed; go through a test mode.
static constexpr uint8_t N_SYS = static_cast<uint8_t>(SystemMode::_Count);
static constexpr bool kSysAllowed[N_SYS][N_SYS] = {
    //                to:  Idle   FuncTest  XIL    Drop   Hover
    /* from Idle     */ { false,  true,     true,  false, false },
    /* from FuncTest */ { true,   false,    true,  true,  true  },
    /* from XIL      */ { true,   true,     false, true,  true  },
    /* from Drop     */ { true,   false,    false, false, false },
    /* from Hover    */ { true,   false,    false, false, false },
};

bool MissionControl::systemTransitionAllowed(SystemMode from, SystemMode to) {
    return kSysAllowed[static_cast<uint8_t>(from)][static_cast<uint8_t>(to)];
}

// Actuator mode transitions can depend on system mode. Defaults:
//   - In Idle (bench/debug): anything goes.
//   - Otherwise: single steps along the linear cascade
//     Duty -> Current -> Speed -> Position -> Pressure -> Thrust
//     (valve position drives nozzle pressure; thrust loop sits atop pressure),
//     so an outer loop is never engaged without its inner loop having run.
bool MissionControl::actuatorTransitionAllowed(SystemMode sys, ActuatorMode from, ActuatorMode to) {
    if (sys == SystemMode::Idle) return true;

    // Disable is always reachable: killing the outputs must never be
    // blocked by cascade rules. Re-engaging from Disable starts at Duty
    // (adjacent step) and climbs from there.
    if (to == ActuatorMode::Disable) return true;

    // Linear chain: adjacent enum values only (bidirectional single steps).
    const int8_t d = static_cast<int8_t>(to) - static_cast<int8_t>(from);
    return d == 1 || d == -1;
}

// ---------------------------------------------------------------------------
MissionControl::MissionControl()
    : actuator_(ActuatorMode::Disable),
      system_(SystemMode::Idle),
      logging_(false),
      locked_(false) {}

// ---- Logging ---------------------------------------------------------------
Result MissionControl::enableLogging() {
    if (logging_) return Result::NoChange;
    logging_ = true;
    if (loggingCb_) loggingCb_(true);
    return Result::Ok;
}

Result MissionControl::disableLogging() {
    if (!logging_) return Result::NoChange;
    logging_ = false;
    if (loggingCb_) loggingCb_(false);
    return Result::Ok;
}

// ---- Actuator mode ----------------------------------------------------------
Result MissionControl::setActuatorMode(ActuatorMode m) {
    if (locked_) return Result::Locked;
    const ActuatorMode cur = actuator_;
    if (m == cur) return Result::NoChange;
    if (!actuatorTransitionAllowed(system_, cur, m)) return Result::InvalidTransition;

    actuator_ = m;
    if (actuatorCb_) actuatorCb_(cur, m);
    return Result::Ok;
}

// ---- System mode ------------------------------------------------------------
Result MissionControl::setSystemMode(SystemMode m) {
    // Idle is the abort/landing escape hatch: reachable even when locked,
    // otherwise a locked flight mode could never be exited.
    if (locked_ && m != SystemMode::Idle) return Result::Locked;
    const SystemMode cur = system_;
    if (m == cur) return Result::NoChange;
    if (!systemTransitionAllowed(cur, m)) return Result::InvalidTransition;

    // Application guard can veto — except Idle, which must stay reachable
    // (same reasoning as the lock escape hatch).
    if (m != SystemMode::Idle && systemGuard_ && !systemGuard_(cur, m))
        return Result::Rejected;

    system_ = m;
    if (systemCb_) systemCb_(cur, m);
    return Result::Ok;
}


} // namespace mc
