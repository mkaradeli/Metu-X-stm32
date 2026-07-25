/*
 * MissionControl.hpp
 *
 *  Created on: Jul 3, 2026
 *      Author: karadeli
 */
#ifndef MISSIONCONTROL_MISSIONCONTROL_HPP_
#define MISSIONCONTROL_MISSIONCONTROL_HPP_

#include "stdint.h"
#include "currentController.h"
//#include "actuatorController.h"
//extern controller_modes controller_mode

enum class system_modes
  : int32_T {
  IDLE = 0,                         // Default value
  TESTFIRE,
  HOVER,
  DROP,
  SHUTDOWN,
  FAULT
};


extern controller_modes controller_mode;
using Function = void (*)(uint32_t time_ms);

class MissionControl {
public:
	MissionControl(volatile bool* log_ready,
	volatile bool* log_recording);
	controller_modes actuator_mode;
	system_modes system_mode;

	controller_modes actuator_mode_desired;
	system_modes system_mode_desired;

	volatile bool* log_ready;
	volatile bool* log_recording;
	bool running;


	uint32_t ops_duration_ms=0; // ms
	uint32_t shutdown_duration_ms=0; // ms
	uint32_t ops_time_counter_ms=0;
	uint32_t ops_start_ms=0;
	uint32_t shutdown_start_ms=0;
	uint32_t shutdown_time_counter_ms = 0;
	uint32_t postShutdownWait_ms = 0;




	void Init(Function taskFunction, Function shutdownFunction);
	bool Start();
	void End();
	void Iter();
	void Toggle();
	Function taskFunction = nullptr;
	Function shutdownFunction = nullptr;

};

extern MissionControl missionControl;




#endif /* MISSIONCONTROL_MISSIONCONTROL_HPP_ */
