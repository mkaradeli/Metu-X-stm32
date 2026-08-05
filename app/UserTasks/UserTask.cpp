/*
 * UserTask.cpp
 *
 *  Created on: Jul 7, 2026
 *      Author: karadeli
 */

#include "UserTask.hpp"
#include "globals.hpp"

controller_modes actuator_mode_desired = controller_modes::POSITION;
uint32_t ops_duration_ms = 10e3; // ms
uint32_t shutdown_duration_ms = 3e3; // ms
uint32_t postShutdownWait_ms = 0e3; //ms


void taskFunction(uint32_t time_ms) { // position control mode

	for (int i=0; i<4; i++)
		actuator[i].actuatorController.rtU.pos_ref_ext= int(time_ms/1000)*100;

}


void shutdownFunction(uint32_t time_ms) { // position control mode
	if (time_ms < 3000)
		for (int i=0; i<4; i++)
			actuator[i].actuatorController.rtY.currentDemand=-2.0;
	else
		for (int i=0; i<4; i++)
			actuator[i].actuatorController.rtY.currentDemand=0.0f;

}
