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
uint32_t postShutdownWait_ms = 30e3; //ms


void taskFunction(uint32_t time_ms) { // position control mode

	for (int i=0; i<4; i++)
		if (time_ms<800)
			actuator[i] .actuatorController.rtU.pos_ref_ext = 100;
		else if (time_ms<1600)
			actuator[i] .actuatorController.rtU.pos_ref_ext = 200;
		else if (time_ms<2400)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 300;
		else if (time_ms<3200)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 400;
		else if (time_ms<4000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 500;
		else if (time_ms<4800)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 600;
		else if (time_ms<5600)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 700;
		else if (time_ms<6400)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 800;
		else if (time_ms<7200)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 900;
		else if (time_ms<8000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 1000;


}


void shutdownFunction(uint32_t time_ms) { // position control mode
	if (time_ms < 3000)
		for (int i=0; i<4; i++)
			actuator[i].actuatorController.rtY.currentDemand=-2.0;
	else
		for (int i=0; i<4; i++)
			actuator[i].actuatorController.rtY.currentDemand=0.0f;

}
