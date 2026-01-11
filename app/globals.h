/*
 * globals.h
 *
 *  Created on: Aug 17, 2025
 *      Author: alica
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#ifdef __cplusplus
extern "C" {
#endif



#define SUBSAMPLE_COUNT 64

// IMU Variables
#include "globals.h"
#include "IMU.h"
#include "sh2_SensorValue.h"
#include "cmsis_os.h"
//#include "encoder.h"


extern sh2_RotationVector_t quaternion;
extern sh2_Accelerometer_t accels;
extern osThreadId imuTaskHandle;
extern uint32_t accel_delay;
extern uint32_t rotation_delay;


//extern encoder_s encoders[4];
#ifdef __cplusplus
}
#endif


#endif /* GLOBALS_H_ */
