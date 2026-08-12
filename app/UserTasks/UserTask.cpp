/*
 * UserTask.cpp
 *
 *  Created on: Jul 7, 2026
 *      Author: karadeli
 */

#include "UserTask.hpp"
#include "globals.hpp"


#define missionID 2
const char logHeader[] = "Nozzle Dia = 2.8, Nozzle 1, Mission ID = 3";


const uint8_t logHeaderSize = sizeof(logHeader);


controller_modes actuator_mode_desired = controller_modes::POSITION;
uint32_t ops_duration_ms = 9e3; // ms
uint32_t shutdown_duration_ms = 3e3; // ms
uint32_t postShutdownWait_ms = 0e3; //ms


void taskFunction(uint32_t time_ms) { // position control mode

	for (int i=0; i<4; i++){
#if missionID==1 // valve opening in steps, 0 deg, 100 deg, 200 deg...
		actuator[i].actuatorController.rtU.pos_ref_ext= int(time_ms/800)*100;
#elif missionID==2 // valve closing in steps, 1080 deg, 1000 deg, 900 deg...
		actuator[i].actuatorController.rtU.pos_ref_ext= (int(ops_duration_ms/800)-int(time_ms/800))*100;
#elif missionID==3
		actuator[i].actuatorController.rtU.pos_ref_ext= 1080;

#endif
	}
}


void shutdownFunction(uint32_t time_ms) { // current control mode
	if (time_ms < 3000)
		for (int i=0; i<4; i++)
			actuator[i].actuatorController.rtY.currentDemand=-2.0;
	else
		for (int i=0; i<4; i++)
			actuator[i].actuatorController.rtY.currentDemand=0.0f;

}
