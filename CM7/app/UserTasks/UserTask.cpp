/*
 * UserTask.cpp
 *
 *  Created on: Jul 7, 2026
 *      Author: karadeli
 */

#include "UserTask.hpp"
#include "globals.hpp"

controller_modes actuator_mode_desired = controller_modes::PRESSURE;
uint32_t ops_duration_ms = 20e3; // ms
uint32_t shutdown_duration_ms = 3e3; // ms


void taskFunction(uint32_t time_ms) { // position control mode
	for (int i=0; i<4; i++)
		actuator[i] .actuatorController.rtU.P_nozzle_demand = 3000 * (static_cast<float>(time_ms%2000)/2000.0f);
}


void shutdownFunction(uint32_t time_ms) { // position control mode
	if (time_ms < 1000)
		for (int i=0; i<4; i++)
			actuator[i].actuatorController.rtU.pos_ref_ext = 20;
	else if (time_ms < 2000)
		for (int i=0; i<4; i++)
			actuator[i].actuatorController.rtU.pos_ref_ext = 1;
	else
		for (int i=0; i<4; i++)
			actuator[i].actuatorController.rtU.pos_ref_ext = 0;
}
