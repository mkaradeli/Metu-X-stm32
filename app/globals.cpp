/*
 * globals.cpp
 *
 *  Created on: Aug 17, 2025
 *      Author: alica
 */

#include "globals.hpp"


// Pressure Sensor Variables

TaskHandle_t psTaskHandle;
uint16_t PSValues[MAX_PS_COUNT] = {0};
PressureSensor psSensors[MAX_PS_COUNT] = {
		PressureSensor(),
		PressureSensor(),
		PressureSensor(),
		PressureSensor(),
		PressureSensor()
};

uint16_t EncoderValues[ADC2_BUF_LEN*ADC2_CH_COUNT] = {0};
