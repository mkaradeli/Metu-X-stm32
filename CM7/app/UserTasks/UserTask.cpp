/*
 * UserTask.cpp
 *
 *  Created on: Jul 7, 2026
 *      Author: karadeli
 */

#include "UserTask.hpp"
#include "globals.hpp"

controller_modes actuator_mode_desired = controller_modes::POSITION;
uint32_t ops_duration_ms = 20e3; // ms
uint32_t shutdown_duration_ms = 3e3; // ms


void taskFunction(uint32_t time_ms) { // position control mode

	for (int i=0; i<4; i++)
		if (time_ms<1000)
			actuator[i] .actuatorController.rtU.pos_ref_ext = 100;
		else if (time_ms<2000)
			actuator[i] .actuatorController.rtU.pos_ref_ext = 200;
		else if (time_ms<3000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 300;
		else if (time_ms<4000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 400;
		else if (time_ms<5000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 500;
		else if (time_ms<6000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 600;
		else if (time_ms<7000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 700;
		else if (time_ms<8000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 800;
		else if (time_ms<9000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 900;
		else if (time_ms<10000)
					actuator[i] .actuatorController.rtU.pos_ref_ext = 1000;

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
