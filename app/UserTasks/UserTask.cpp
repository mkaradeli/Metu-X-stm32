/*
 * UserTask.cpp
 *
 *  Created on: Jul 7, 2026
 *      Author: karadeli
 *
 *  ALL MISSIONS LIVE HERE. MissionControl.* must not be edited.
 *
 *  To add a mission:
 *    1. write a task function  (t = ms since the mission started firing)
 *    2. write a shutdown function, or reuse valveShutdown()
 *    3. add one row to missionTable[] below
 *  Select the armed mission at run time over UART:  SEL 2   /  SEL HOVER
 */

#include "UserTask.hpp"
#include "globals.hpp"
#include "main.h"            // SAFETY_CONNECTOR_GPIO_Port / _Pin
#include "platformController.h"
/* ======================================================================== */
/*  Safety connector: reads 0 when connected, 1 when pulled out             */
/* ======================================================================== */
bool safetyConnectorReleased() {
	return HAL_GPIO_ReadPin(SAFETY_CONNECTOR_GPIO_Port, SAFETY_CONNECTOR_Pin)
	       == GPIO_PIN_SET;
}

/* ======================================================================== */
/*  Shared helpers                                                          */
/* ======================================================================== */
static inline void setAllValves(float angle_deg) {
	for (int i = 0; i < 4; i++)
		actuator[i].actuatorController.rtU.pos_ref_ext = angle_deg;
}

/* Standard shutdown: drive the valves shut on current for 3 s, then relax. */
static void valveShutdown(uint32_t time_ms) {   // current control mode
	platform_controller.rtU.Dropped = false;

	if (time_ms < 3000)
		for (int i = 0; i < 4; i++)
			actuator[i].actuatorController.rtY.currentDemand = -0.5f;
	else
		for (int i = 0; i < 4; i++)
			actuator[i].actuatorController.rtY.currentDemand = 0.0f;
}

/* ======================================================================== */
/*  Mission 0 : TESTFIRE, valve opening in steps 0, 100, 200 deg ...        */
/* ======================================================================== */
static const uint32_t TESTFIRE_STEP_MS  = 800;
static const uint32_t TESTFIRE_OPS_MS   = 9000;

static void testfireOpenTask(uint32_t time_ms) {   // position control mode
	setAllValves((float)(int)(time_ms / TESTFIRE_STEP_MS) * 100.0f);
}

/* ======================================================================== */
/*  Mission 1 : TESTFIRE, valve closing in steps 1080, 1000, 900 deg ...    */
/* ======================================================================== */
static void testfireCloseTask(uint32_t time_ms) {  // position control mode
	const int steps = (int)(TESTFIRE_OPS_MS / TESTFIRE_STEP_MS)
	                - (int)(time_ms / TESTFIRE_STEP_MS);
	setAllValves((float)steps * 100.0f);
	if (time_ms > 9000) setAllValves(10.0f);
}

/* ======================================================================== */
/*  Mission 2 : TESTFIRE, constant full open                               */
/* ======================================================================== */
static void testfireHoldTask(uint32_t time_ms) {   // position control mode
	setAllValves(1080.0f);
	if (time_ms > 9000) setAllValves(10.0f);
}

/* ======================================================================== */
/*  Mission 3 : SAFE_DISCHARGE, hold all four valves at 100 deg            */
/* ======================================================================== */
static void safeDischargeTask(uint32_t time_ms) {  // position control mode
	(void)time_ms;
	setAllValves(20.0f);
}

/* ======================================================================== */
/*  Mission 4 : HOVER                                                      */
/*  Mission 5 : DROP                                                       */
/*  Both are armed by the button (logging starts there) and only fire when */
/*  the safety connector is pulled.                                        */
/* ======================================================================== */
uint32_t last_time_ms=10000;
static void hoverTask(uint32_t time_ms) {
	mission_mode = mission_modes::HOVER;
	platform_controller.rtU.Dropped = true;

	{
		actuator[0].actuatorController.rtU.F_demand = platform_controller.rtY.LeftThrustCmd;
		actuator[1].actuatorController.rtU.F_demand = platform_controller.rtY.FrontThrustCmd;
		actuator[2].actuatorController.rtU.F_demand = platform_controller.rtY.RightThrustCmd;
		actuator[3].actuatorController.rtU.F_demand = platform_controller.rtY.BackThrustCmd;
	}
}

static void dropTask(uint32_t time_ms) {
	mission_mode = mission_modes::DROP;
	platform_controller.rtU.Dropped = true;
	{
		actuator[0].actuatorController.rtU.F_demand = platform_controller.rtY.LeftThrustCmd;
		actuator[1].actuatorController.rtU.F_demand = platform_controller.rtY.FrontThrustCmd;
		actuator[2].actuatorController.rtU.F_demand = platform_controller.rtY.RightThrustCmd;
		actuator[3].actuatorController.rtU.F_demand = platform_controller.rtY.BackThrustCmd;
	}
}

// MISSION 6

static void ThrustDemand_const(uint32_t time_ms){
	if (time_ms < 5500 ) {
		for (int i=0; i<4; i++) {
			actuator[i].actuatorController.rtU.F_demand = 50;
		}
	}
	else
	{
		for (int i=0; i<4; i++) {
					actuator[i].actuatorController.rtU.F_demand = 5;
				}
	}

}


/* ======================================================================== */
/*  THE MISSION TABLE                                                       */
/*                                                                          */
/*  name | kind | actuator mode | task | shutdown | ops_ms | shutdown_ms |  */
/*  postShutdown_ms | log_estimate_ms | wait_safety_release |               */
/*  abort_on_safety_connect | arm_timeout_ms | header                       */
/* ======================================================================== */
extern const MissionDef missionTable[] = {

 { "TF_OPEN",  system_modes::TESTFIRE, controller_modes::POSITION,
   testfireOpenTask,  valveShutdown,
   TESTFIRE_OPS_MS, 3000, 0,
   15000,  false, false, 0,
   "Nozzle Dia = 2.8, Nozzle 1, Mission = TESTFIRE opening steps" },

 { "TF_CLOSE", system_modes::TESTFIRE, controller_modes::POSITION,
   testfireCloseTask, valveShutdown,
   TESTFIRE_OPS_MS, 3000, 0,
   15000,  false, false, 0,
   "Nozzle Dia = 2.8, Nozzle 1, Mission = TESTFIRE closing steps" },

 { "TF_HOLD",  system_modes::TESTFIRE, controller_modes::POSITION,
   testfireHoldTask,  valveShutdown,
   TESTFIRE_OPS_MS, 3000, 0,
   15000,  false, false, 0,
   "Nozzle Dia = 2.8, Nozzle 1, Mission = TESTFIRE full open" },

 { "SAFE_DISCHARGE", system_modes::SAFE_DISCHARGE, controller_modes::POSITION,
   safeDischargeTask, valveShutdown,
   30000, 3000, 0,
   35000,  false, false, 0,
   "Mission = SAFE_DISCHARGE, all valves 100 deg" },

 /* ---- flight missions: ARM on button, FIRE on safety connector release ---
  * ops_duration_ms is a hard cut-off, keep it above the expected flight
  * time. log_estimate_ms is only used to pre-allocate the SD file, going
  * past it costs a bit of latency but does not stop the log.              */
 { "HOVER", system_modes::HOVER, controller_modes::POSITION,
   hoverTask, valveShutdown,
   8000, 1000, 2000,
   300000, true,  true,  0,
   "Mission = HOVER" },

 { "DROP",  system_modes::DROP,  controller_modes::POSITION,
   dropTask,  valveShutdown,
   180000, 5000, 2000,
   120000, true,  true,  0,
   "Mission = DROP" },

	{ "CONST_THRUST",  system_modes::TESTFIRE,  controller_modes::FORCE,
			ThrustDemand_const,  valveShutdown,
	6000, 1000, 0,
	300000, true,  false,  0,
	 "Mission = constant force demand 50 N, bismillah. 17.agustos" },
};

extern const uint8_t missionTableCount =
		(uint8_t)(sizeof(missionTable) / sizeof(missionTable[0]));

/* Mission selected at boot, before any UART SEL command. */


// HOVER 4
// DROP 5
extern const uint8_t defaultMissionIndex = 4;   // TF_OPEN

/* The old global logHeader[] / logHeaderSize are gone: sd_task now takes the
 * header text from the selected mission row above. Delete their extern
 * declarations from globals.hpp. */
