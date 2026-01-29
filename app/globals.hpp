
/*
 * globals.h
 *
 *  Created on: Aug 17, 2025
 *      Author: alica
 */

#ifndef GLOBALS_HPP_
#define GLOBALS_HPP_

#define MAX_MOTOR_COUNT 4
#define MAX_PS_COUNT 5

#define PI 3.1415926536

#define ADC2_BUF_LEN     64
#define ADC2_CH_COUNT    4

#include <stdint.h>

#include "PressureSensor.hpp"
#include "sh2_SensorValue.h"

#include "cmsis_os.h"




// Pressure Sensor Variables
extern TaskHandle_t psTaskHandle;
extern uint16_t PSValues[MAX_PS_COUNT];
extern PressureSensor psSensors[MAX_PS_COUNT];

// Motor Variables
extern uint16_t EncoderValues[ADC2_CH_COUNT*ADC2_BUF_LEN];



//extern int lastWriteDone;
#endif /* GLOBALS_HPP_ */
