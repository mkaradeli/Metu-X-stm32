/*
 * MissionControl.cpp
 *
 *  Created on: Jul 3, 2026
 *      Author: karadeli
 *
 *  Mission sequencer. Missions themselves live in UserTask.cpp.
 *
 *  State machine
 *  -------------
 *      IDLE --Start()--> [wait_safety_release ? ARMED : <kind>]
 *      ARMED  : logging is running, actuators DISABLEd.
 *               Fires when the safety connector is pulled (pin = 1,
 *               held for safety_debounce_ms). Button/UART stop disarms.
 *      <kind> : ops callback runs with t = ms since fire.
 *      SHUTDOWN: shutdown callback, then postShutdownWait, then IDLE.
 *
 *  Safe discharge override
 *  -----------------------
 *  SafeDischarge() cuts into any state, including FAULT and mid-flight, and
 *  runs the mission whose kind is SAFE_DISCHARGE. It does not touch the
 *  selection, so an open log file keeps its identity and sd_task does not
 *  roll it mid-abort. The distinction is `selected` (what the operator has
 *  queued) versus `active` (what is executing now).
 *
 *  ISR notes
 *  ---------
 *  Iter() runs in the pressure-loop ISR while Start()/End()/HandleCommand()
 *  run in task context. Every transition therefore writes its timestamps
 *  first and publishes `system_mode` last, behind a barrier: an ISR that
 *  preempts mid-transition sees either the old state or a fully consistent
 *  new one, never a new mode with a stale ops_start_ms.
 */

#include "MissionControl.hpp"
#include "stm32h7xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ---------------- small local string helpers (no <ctype.h> locale) ------- */
static char up(char c) { return (c >= 'a' && c <= 'z') ? (char)(c - 32) : c; }

static bool ieq(const char *a, const char *b) {
	while (*a && *b) {
		if (up(*a) != up(*b)) return false;
		++a; ++b;
	}
	return *a == 0 && *b == 0;
}

/* first token of `s`, copied into buf; returns pointer to the rest */
static const char* token(const char *s, char *buf, size_t n) {
	while (*s == ' ' || *s == '\t') ++s;
	size_t i = 0;
	while (*s && *s != ' ' && *s != '\t' && *s != '\r' && *s != '\n') {
		if (i + 1 < n) buf[i++] = *s;
		++s;
	}
	buf[i] = 0;
	while (*s == ' ' || *s == '\t') ++s;
	return s;
}

static bool all_digits(const char *s) {
	if (*s == 0) return false;
	for (; *s; ++s) if (*s < '0' || *s > '9') return false;
	return true;
}
/* ------------------------------------------------------------------------ */

const char *MissionControl::ErrorText(mission_error_t e) {
	switch (e) {
	case mission_error_t::NONE:                              return "";
	case mission_error_t::BUSY:                               return "busy";
	case mission_error_t::BAD_INDEX:                          return "bad index";
	case mission_error_t::NO_SUCH_MISSION:                    return "no such mission";
	case mission_error_t::NOT_IDLE:                           return "not idle";
	case mission_error_t::LOG_NOT_READY:                      return "log not ready";
	case mission_error_t::MISSION_NOT_VALID:                  return "mission not valid";
	case mission_error_t::SAFETY_ALREADY_RELEASED:            return "safety connector already released";
	case mission_error_t::NO_SAFE_DISCHARGE_MISSION:          return "no SAFE_DISCHARGE mission in table";
	case mission_error_t::SAFE_DISCHARGE_MISSION_NOT_VALID:   return "SAFE_DISCHARGE mission not valid";
	case mission_error_t::SAFE_DISCHARGE_ACTIVE:              return "safe discharge";
	case mission_error_t::ARM_TIMEOUT:                        return "arm timeout";
	case mission_error_t::SAFETY_REINSERTED:                  return "safety connector re-inserted";
	case mission_error_t::GO_NO_GO_FAIL:                      return "go/no-go check failed";
	default:                                                  return "?";
	}
}

MissionControl::MissionControl(volatile bool *log_ready,
		volatile bool *log_recording) {
	this->system_mode   = system_modes::IDLE;
	this->actuator_mode = controller_modes::DISABLE;
	controller_mode     = this->actuator_mode;
	this->log_ready     = log_ready;
	this->log_recording = log_recording;
}

void MissionControl::Init() {
	this->selected_index = 0;
	this->active = nullptr;
	this->system_mode = system_modes::IDLE;
	SetActuatorMode(controller_modes::DISABLE);

	/* Locate the safe-discharge mission once. First row whose kind is
	 * SAFE_DISCHARGE wins. */
	safe_index = 0xFFu;
	for (uint8_t i = 0; i < missionTableCount; ++i) {
		if (missionTable[i].kind == system_modes::SAFE_DISCHARGE) {
			safe_index = i;
			break;
		}
	}
}

/* ---------------- selection --------------------------------------------- */

const MissionDef* MissionControl::Selected() const {
	if (missionTableCount == 0 || selected_index >= missionTableCount)
		return nullptr;
	return &missionTable[selected_index];
}

const char* MissionControl::SelectedName() const {
	const MissionDef *m = Selected();
	return (m && m->name) ? m->name : "<none>";
}

const char* MissionControl::SelectedHeader() const {
	const MissionDef *m = Selected();
	return (m && m->header) ? m->header : "";
}

const char* MissionControl::ActiveName() const {
	if (active == nullptr)  return "-";          /* nothing executing */
	return active->name ? active->name : "?";
}

/* Pre-allocation length. Falls back to the mission's own duration budget
 * when no explicit estimate is given. Returns 0 for open-ended missions,
 * which sd_task reads as "do not pre-allocate". */
uint32_t MissionControl::LogPreallocMs() const {
	const MissionDef *m = Selected();
	if (m == nullptr) return 0;
	if (m->log_estimate_ms) return m->log_estimate_ms;
	if (m->ops_duration_ms == MISSION_NO_TIMEOUT) return 0;
	return m->ops_duration_ms + m->shutdown_duration_ms + m->postShutdownWait_ms;
}

bool MissionControl::Select(uint8_t index) {
	if (running || system_mode != system_modes::IDLE) {
		last_error = mission_error_t::BUSY;
		return false;
	}
	if (index >= missionTableCount) {
		last_error = mission_error_t::BAD_INDEX;
		return false;
	}
	selected_index = index;
	last_error = mission_error_t::NONE;
	go_no_go_enabled = (missionTable[index].kind == system_modes::TESTFIRE)
	                    ? GoNoGo::TESTFIRE_DEFAULT : GoNoGo::ALL;
	return true;
}

bool MissionControl::Select(const char *name) {
	for (uint8_t i = 0; i < missionTableCount; ++i) {
		if (missionTable[i].name && ieq(missionTable[i].name, name))
			return Select(i);
	}
	last_error = mission_error_t::NO_SUCH_MISSION;
	return false;
}

const char* MissionControl::StateName() const {
	switch (system_mode) {
	case system_modes::IDLE:           return "IDLE";
	case system_modes::ARMED:          return "ARMED";
	case system_modes::TESTFIRE:       return "TESTFIRE";
	case system_modes::HOVER:          return "HOVER";
	case system_modes::DROP:           return "DROP";
	case system_modes::SAFE_DISCHARGE: return "SAFE_DISCHARGE";
	case system_modes::SHUTDOWN:       return "SHUTDOWN";
	default:                           return "FAULT";
	}
}

/* ---------------- internals --------------------------------------------- */

void MissionControl::SetActuatorMode(controller_modes m) {
	actuator_mode   = m;
	controller_mode = m;
}

bool MissionControl::SafetyReleasedDebounced() {
	if (!safetyConnectorReleased()) {
		safety_release_pending = false;
		return false;
	}
	if (!safety_release_pending) {
		safety_release_pending  = true;
		safety_release_first_ms = uwTick;
		return false;
	}
	return (uint32_t)(uwTick - safety_release_first_ms) >= safety_debounce_ms;
}

/* Enter the ops phase of whatever `active` points at. */
void MissionControl::BeginOps() {
	const MissionDef *m = active;
	if (m == nullptr) { End(); return; }

	ops_duration_ms      = m->ops_duration_ms;
	shutdown_duration_ms = m->shutdown_duration_ms;
	postShutdownWait_ms  = m->postShutdownWait_ms;

	ops_start_ms        = uwTick;
	ops_time_counter_ms = 0;
	req_shutdown        = false;
	firing              = true;
	SetActuatorMode(m->actuator_mode);

	__DMB();
	system_mode = m->kind;          /* publish last */
}

void MissionControl::BeginShutdown() {
	const MissionDef *m = active;

	shutdown_start_ms        = uwTick;
	shutdown_time_counter_ms = 0;
	req_shutdown             = false;
	firing                   = true;
	SetActuatorMode(controller_modes::CURRENT);

	__DMB();
	system_mode = system_modes::SHUTDOWN;

	if (m && m->shutdown) m->shutdown(0);
}

/* ---------------- start / stop ------------------------------------------ */

bool MissionControl::Start() {
	last_error = mission_error_t::NONE;

	if (running || system_mode != system_modes::IDLE) {
		last_error = mission_error_t::NOT_IDLE;
		return false;
	}
	if (!*log_ready) {
		last_error = mission_error_t::LOG_NOT_READY;
		return false;
	}

	const MissionDef *m = Selected();
	if (m == nullptr || m->task == nullptr || m->shutdown == nullptr) {
		last_error = mission_error_t::MISSION_NOT_VALID;
		return false;
	}

	/* Any bit enabled here but not currently healthy in go_no_go_status
	 * blocks arming. A disabled bit is never checked, healthy or not. */
	if ((go_no_go_enabled & ~go_no_go_status) != 0) {
		last_error = mission_error_t::GO_NO_GO_FAIL;
		return false;
	}

	active = m;

	/* logging starts here, at the trigger, for every mission. The file was
	 * already created and pre-allocated by sd_task for this mission. */
	*log_recording = true;
	running = true;

	if (m->wait_safety_release) {
		/* refuse to arm with the pin already out: the vehicle would fire the
		 * instant the button is pressed. */
		if (safetyConnectorReleased()) {
			*log_recording = false;
			running = false;
			active  = nullptr;
			last_error = mission_error_t::SAFETY_ALREADY_RELEASED;
			return false;
		}
		ops_duration_ms      = m->ops_duration_ms;
		shutdown_duration_ms = m->shutdown_duration_ms;
		postShutdownWait_ms  = m->postShutdownWait_ms;

		armed_start_ms         = uwTick;
		armed_time_counter_ms  = 0;
		ops_time_counter_ms    = 0;
		firing                 = false;
		safety_release_pending = false;
		SetActuatorMode(controller_modes::DISABLE);

		__DMB();
		system_mode = system_modes::ARMED;
	} else {
		BeginOps();
	}
	return true;
}

void MissionControl::End() {
	SetActuatorMode(controller_modes::DISABLE);
	firing  = false;
	running = false;
	active  = nullptr;
	*log_recording = false;
	safety_release_pending = false;
	req_shutdown = false;

	__DMB();
	system_mode = system_modes::IDLE;
}

void MissionControl::Toggle() {
	if (!running) this->Start();
	else          this->End();
}

/* ---------------- safe discharge override -------------------------------
 * Valid from every state, including FAULT and mid-flight. Never fails for a
 * reason the operator would consider optional: a missing SD card does not
 * stop it, and it deliberately does not touch the selection so that an open
 * log file keeps recording into the same file.
 * ------------------------------------------------------------------------ */

bool MissionControl::SafeDischarge() {
	if (safe_index >= missionTableCount) {
		last_error = mission_error_t::NO_SAFE_DISCHARGE_MISSION;
		return false;
	}
	const MissionDef *m = &missionTable[safe_index];
	if (m->task == nullptr || m->shutdown == nullptr) {
		last_error = mission_error_t::SAFE_DISCHARGE_MISSION_NOT_VALID;
		return false;
	}

	/* already discharging: leave the running timer alone */
	if (active == m && system_mode == system_modes::SAFE_DISCHARGE) return true;

	/* drop anything the operator queued a moment ago */
	req_start = req_stop = req_toggle = req_shutdown = false;

	if (!running) {
		/* Cold start. Record it if a file happens to be open, but never
		 * block the discharge on the SD card. */
		*log_recording = *log_ready;
		running = true;
	}

	active = m;
	BeginOps();
	last_error = mission_error_t::SAFE_DISCHARGE_ACTIVE;
	return true;
}

void MissionControl::ServiceRequests() {
	if (req_discharge) { req_discharge = false; SafeDischarge(); return; }
	if (req_toggle)    { req_toggle = false; Toggle(); }
	if (req_start)     { req_start  = false; if (!running) Start(); }
	if (req_stop)      { req_stop   = false; if (running)  End();   }
}

/* ---------------- periodic ---------------------------------------------- */

void MissionControl::Iter() {
	ServiceRequests();

	const MissionDef *m = active;

	switch (system_mode) {
	case system_modes::IDLE:
	case system_modes::FAULT:
		break;

	case system_modes::ARMED:
		if (m == nullptr) { End(); break; }
		armed_time_counter_ms = uwTick - armed_start_ms;

		if (SafetyReleasedDebounced()) {
			BeginOps();
		} else if (m->arm_timeout_ms != 0
		        && m->arm_timeout_ms != MISSION_NO_TIMEOUT
		        && armed_time_counter_ms >= m->arm_timeout_ms) {
			last_error = mission_error_t::ARM_TIMEOUT;
			End();
		}
		break;

	case system_modes::TESTFIRE:
	case system_modes::HOVER:
	case system_modes::DROP:
	case system_modes::SAFE_DISCHARGE:
		if (m == nullptr) { BeginShutdown(); break; }
		ops_time_counter_ms = uwTick - ops_start_ms;

		if (m->abort_on_safety_connect && !safetyConnectorReleased()) {
			last_error = mission_error_t::SAFETY_REINSERTED;
			BeginShutdown();
		} else if (req_shutdown) {
			BeginShutdown();
		} else if (m->ops_duration_ms == MISSION_NO_TIMEOUT
		        || ops_time_counter_ms < m->ops_duration_ms) {
			m->task(ops_time_counter_ms);
		} else {
			BeginShutdown();
		}
		break;

	case system_modes::SHUTDOWN:
		ops_time_counter_ms      = uwTick - ops_start_ms;
		shutdown_time_counter_ms = uwTick - shutdown_start_ms;

		if (m && shutdown_time_counter_ms < shutdown_duration_ms) {
			m->shutdown(shutdown_time_counter_ms);
		} else if (shutdown_time_counter_ms
		           < shutdown_duration_ms + postShutdownWait_ms) {
			SetActuatorMode(controller_modes::DISABLE);
			firing = false;
		} else {
			End();
		}
		break;

	default:
		SetActuatorMode(controller_modes::DISABLE);
		*log_recording = false;
		running = false;
		firing  = false;
		active  = nullptr;
		system_mode = system_modes::FAULT;
		break;
	}
}

/* ---------------- UART front-end ---------------------------------------- */

int MissionControl::ListMissions(char *out, size_t n) const {
	int w = 0;
	w += snprintf(out + w, (w < (int)n) ? n - w : 0,
	              "idx  name              ops[ms]  sd[ms]  safety\r\n");
	for (uint8_t i = 0; i < missionTableCount && w < (int)n; ++i) {
		const MissionDef &d = missionTable[i];
		w += snprintf(out + w, (w < (int)n) ? n - w : 0,
		              "%c%2u  %-16s %8lu %7lu  %s%s\r\n",
		              (i == selected_index) ? '*' : ' ', (unsigned)i,
		              d.name ? d.name : "?",
		              (unsigned long)d.ops_duration_ms,
		              (unsigned long)d.shutdown_duration_ms,
		              d.wait_safety_release ? "yes" : "no",
		              (i == safe_index) ? "  <- DISCHARGE" : "");
	}
	return w;
}

int MissionControl::Report(char *out, size_t n) const {
	return snprintf(out, n,
	    "state=%s sel=%u:%s active=%s log=%s t_ops=%lu t_arm=%lu safety=%s err=%s\r\n",
	    StateName(), (unsigned)selected_index, SelectedName(), ActiveName(),
	    (*log_recording) ? "REC" : "off",
	    (unsigned long)ops_time_counter_ms,
	    (unsigned long)armed_time_counter_ms,
	    safetyConnectorReleased() ? "OUT" : "IN",
	    ErrorText(last_error));
}

/*  Commands (case insensitive):
 *      LIST                 list compile-time missions
 *      SEL <idx|name>       select the armed mission (idle only)
 *      ARM <name>           trigger (same as the button), name must match
 *      STOP | ABORT | END   stop / disarm
 *      SHUTDOWN             graceful early shutdown of a running mission
 *      DISCHARGE | VENT     safe discharge, valid in any state
 *      STATUS               one-line report
 *  Returns true if the command was recognised.
 */
bool MissionControl::HandleCommand(const char *cmd, char *reply, size_t n) {
	char verb[16];
	char arg[32];
	const char *rest = token(cmd, verb, sizeof(verb));
	token(rest, arg, sizeof(arg));

	if (verb[0] == 0) return false;

	/* checked before anything else so it still answers in FAULT */
	if (ieq(verb, "DISCHARGE") || ieq(verb, "VENT") || ieq(verb, "SAFE")) {
		if (safe_index >= missionTableCount) {
			snprintf(reply, n, "DISCHARGE FAIL: no SAFE_DISCHARGE mission\r\n");
		} else {
			RequestSafeDischarge();
			snprintf(reply, n, "DISCHARGE requested (%s)\r\n",
			         missionTable[safe_index].name);
		}
		return true;
	}
	if (ieq(verb, "LIST")) {
		ListMissions(reply, n);
		return true;
	}
	if (ieq(verb, "SEL") || ieq(verb, "MISSION")) {
		bool ok = all_digits(arg) ? Select((uint8_t)atoi(arg)) : Select(arg);
		snprintf(reply, n, "%s -> %u:%s%s%s\r\n", ok ? "SEL OK" : "SEL FAIL",
		         (unsigned)selected_index, SelectedName(),
		         ok ? "" : " : ", ok ? "" : ErrorText(last_error));
		return true;
	}
	if (ieq(verb, "ARM") || ieq(verb, "START") || ieq(verb, "GO")) {
		/* The mission name has to be repeated. A stray byte burst on the link
		 * must never be able to fire a testfire, which starts the instant
		 * Start() returns. */
		if (arg[0] == 0 || !ieq(arg, SelectedName())) {
			snprintf(reply, n, "START FAIL: send 'ARM %s' to confirm\r\n",
			         SelectedName());
			return true;
		}
		bool ok = Start();
		snprintf(reply, n, "%s %s state=%s%s%s\r\n", ok ? "START OK" : "START FAIL",
		         SelectedName(), StateName(), ok ? "" : " : ", ok ? "" : ErrorText(last_error));
		return true;
	}
	if (ieq(verb, "STOP") || ieq(verb, "ABORT") || ieq(verb, "END")) {
		End();
		snprintf(reply, n, "STOPPED state=%s\r\n", StateName());
		return true;
	}
	if (ieq(verb, "SHUTDOWN")) {
		RequestShutdown();
		snprintf(reply, n, "SHUTDOWN requested\r\n");
		return true;
	}
	if (ieq(verb, "STATUS") || ieq(verb, "?")) {
		Report(reply, n);
		return true;
	}
	snprintf(reply, n, "unknown cmd '%s'\r\n", verb);
	return false;
}
