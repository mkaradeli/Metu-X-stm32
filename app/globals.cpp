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
constexpr int encoderSigns[4] = {1,1,-1,1};


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
		{&psSensors[0], &adc_dma_buf_encoder[0],&motors[0], encoderSigns[0]},
		{&psSensors[1], &adc_dma_buf_encoder[1],&motors[1], encoderSigns[1]},
		{&psSensors[2], &adc_dma_buf_encoder[2],&motors[2], encoderSigns[2]},
		{&psSensors[3], &adc_dma_buf_encoder[3],&motors[3], encoderSigns[3]}
};

PressureSensor* Actuator::manifold = &psSensors[4];

LoadCell loadCell(&adc_dma_buf_pressure[3]);


const uint16_t logFormatId = 16;


const uint16_t sensorDataLength = sizeof(SensorData_t);

static_assert(sensorDataLength == 317,"aaaaaaa");


Lidar lidar = Lidar(&huart6);

const float ValveFitPressureRatios[4][12] = {{0, 0.057631480834633, 0.171997382575738, 0.280936562859588, 0.373770506465553, 0.460925365691390, 0.549715259309827, 0.627065318978796, 0.673531606657655, 0.705225494170721, 0.719071509814285, 0.735751202120408},
											 {0, 0.057631480834633, 0.171997382575738, 0.280936562859588, 0.373770506465553, 0.460925365691390, 0.549715259309827, 0.627065318978796, 0.673531606657655, 0.705225494170721, 0.719071509814285, 0.735751202120408},
											 {0, 0.057631480834633, 0.171997382575738, 0.280936562859588, 0.373770506465553, 0.460925365691390, 0.549715259309827, 0.627065318978796, 0.673531606657655, 0.705225494170721, 0.719071509814285, 0.735751202120408},
											 {0, 0.057631480834633, 0.171997382575738, 0.280936562859588, 0.373770506465553, 0.460925365691390, 0.549715259309827, 0.627065318978796, 0.673531606657655, 0.705225494170721, 0.719071509814285, 0.735751202120408}};
gnc::AltitudeEstimator altEstimator;


static_assert(encoderSigns[0]==1 or encoderSigns[0]==-1,"degerler sadece +1 ve -1 olabilir.");
static_assert(encoderSigns[1]==1 or encoderSigns[1]==-1,"degerler sadece +1 ve -1 olabilir.");
static_assert(encoderSigns[2]==1 or encoderSigns[2]==-1,"degerler sadece +1 ve -1 olabilir.");
static_assert(encoderSigns[3]==1 or encoderSigns[3]==-1,"degerler sadece +1 ve -1 olabilir.");
