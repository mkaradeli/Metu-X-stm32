/*
 * IMU.c
 *
 *  Created on: Aug 4, 2025
 *      Author: alica
 */

#include "IMU.h"

#include <stdio.h>
#include "string.h"

#include "tim.h"

#include "sh2.h"
#include "sh2_util.h"
#include "euler.h"
#include "sh2_err.h"
#include "sh2_SensorValue.h"
#include "sh2_hal_init.h"
#include "euler.h"

static sh2_SensorValue_t value;
static bool resetOccurred = false;

static sh2_Hal_t *pSh2Hal = 0;
sh2_RotationVector_t quaternion = {0};
sh2_Accelerometer_t accels = {0};
// Print a sensor event to the console
static void printEvent(const sh2_SensorEvent_t * event)
{
    static int rc;
    static uint32_t accel_delay_flag;
    static uint32_t rotation_delay_flag;
    rc = sh2_decodeSensorEvent(&value, event);
    if (rc != SH2_OK) {
        // printf("Error decoding sensor event: %d\n", rc);
        return;
    }

    switch (value.sensorId) {
    	case SH2_LINEAR_ACCELERATION:
    		accels = value.un.linearAcceleration;
    		accel_delay = micros()-accel_delay_flag;
    		accel_delay_flag = micros();
    		break;
    	case SH2_GAME_ROTATION_VECTOR:
    	    quaternion = value.un.gameRotationVector;
    	    rotation_delay = micros()-rotation_delay_flag;
    	    rotation_delay_flag = micros();
    	    break;
    	default:
    		break;
    }
}

static void sensorHandler(void * cookie, sh2_SensorEvent_t *pEvent)
{
    printEvent(pEvent);
}

static void eventHandler(void * cookie, sh2_AsyncEvent_t *pEvent)
{
    // If we see a reset, set a flag so that sensors will be reconfigured.
    if (pEvent->eventId == SH2_RESET) {
        resetOccurred = true;
    }
    else if (pEvent->eventId == SH2_SHTP_EVENT) {
        // printf("EventHandler  id:SHTP, %d\n", pEvent->shtpEvent);
    }
    else if (pEvent->eventId == SH2_GET_FEATURE_RESP) {
        // printf("EventHandler Sensor Config, %d\n", pEvent->sh2SensorConfigResp.sensorId);
    }
    else {
        // printf("EventHandler, unknown event Id: %d\n", pEvent->eventId);
    }
}

void IMU_Init(void){
	int status;

	printf("\n\nCEVA SH2 Sensor Hub Demo.\n\n");

	pSh2Hal = sh2_hal_init();
	status = sh2_open(pSh2Hal, eventHandler, NULL);
	if (status != SH2_OK) {
	  printf("Error, %d, from sh2_open.\n", status);
	}
	sh2_setSensorCallback(sensorHandler, NULL);
	osDelay(100);
	sh2_SensorConfig_t accel_config = {.reportInterval_us = 1};
	status = sh2_setSensorConfig(SH2_LINEAR_ACCELERATION, &accel_config);
	if (status != 0) {
	  printf("Error while enabling accelerometer sensor");
	}
	osDelay(100);
	sh2_SensorConfig_t rotation_config = {.reportInterval_us = 2500};
	status = sh2_setSensorConfig(SH2_GAME_ROTATION_VECTOR, &rotation_config);
	if (status != 0) {
	  printf("Error while enabling rotation sensor");
	}

}



