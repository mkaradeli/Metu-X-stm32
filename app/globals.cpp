/*
 * globals.cpp
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */
#include "globals.hpp"
#include "main.h"
#include "PressureSensor.hpp"
#include "shared_memory.h"



extern uint16_t adc_dma_buf_encoder[8*4];
extern uint16_t adc_dma_buf_pressure[5];

static Motor motors[4] = {
		Motor(LEFT_EN_1_GPIO_Port, LEFT_EN_1_Pin, RIGHT_EN_1_GPIO_Port, RIGHT_EN_1_Pin, &htim1, TIM_CHANNEL_2),
		Motor(LEFT_EN_2_GPIO_Port, LEFT_EN_2_Pin, RIGHT_EN_2_GPIO_Port, RIGHT_EN_2_Pin, &htim1, TIM_CHANNEL_1),
		Motor(LEFT_EN_3_GPIO_Port, LEFT_EN_3_Pin, RIGHT_EN_3_GPIO_Port, RIGHT_EN_3_Pin, &htim1, TIM_CHANNEL_4),
		Motor(LEFT_EN_4_GPIO_Port, LEFT_EN_4_Pin, RIGHT_EN_4_GPIO_Port, RIGHT_EN_4_Pin, &htim1, TIM_CHANNEL_3),
};



static PressureSensor psSensors[5] = {
		PressureSensor(&adc_dma_buf_pressure[0]),
		PressureSensor(&adc_dma_buf_pressure[1]),
		PressureSensor(&adc_dma_buf_pressure[2]),
		PressureSensor(&adc_dma_buf_pressure[3]),
		PressureSensor(&adc_dma_buf_pressure[4])
};

Actuator actuator[4] = {
		{&psSensors[0], &adc_dma_buf_encoder[0],&motors[0]},
		{&psSensors[1], &adc_dma_buf_encoder[1],&motors[1]},
		{&psSensors[2], &adc_dma_buf_encoder[2],&motors[2]},
		{&psSensors[3], &adc_dma_buf_encoder[3],&motors[3]}
};

PressureSensor* Actuator::manifold = &psSensors[4];

LoadCell loadCell(&adc_dma_buf_pressure[3]);


const uint16_t logFormatId = 14;
const char logHeader[] = "test";
const uint8_t logHeaderSize = sizeof(logHeader);
const uint16_t sensorDataLength = sizeof(SensorData_t);
