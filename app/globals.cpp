/*
 * globals.cpp
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */


#include <shared_memory.h>
#include "globals.hpp"
#include "main.h"
#include "PressureSensor.hpp"

constexpr int encoderSigns[4] = {1,1,1,1};


extern uint16_t adc_dma_buf_encoder[8*4];
extern uint16_t adc_dma_buf_pressure[5];

static Motor motors[4] = {
		Motor(LEFT_EN_0_GPIO_Port, LEFT_EN_0_Pin, RIGHT_EN_0_GPIO_Port, RIGHT_EN_0_Pin, &htim1, TIM_CHANNEL_2),
		Motor(LEFT_EN_1_GPIO_Port, LEFT_EN_1_Pin, RIGHT_EN_1_GPIO_Port, RIGHT_EN_1_Pin, &htim1, TIM_CHANNEL_1),
		Motor(LEFT_EN_2_GPIO_Port, LEFT_EN_2_Pin, RIGHT_EN_2_GPIO_Port, RIGHT_EN_2_Pin, &htim1, TIM_CHANNEL_4),
		Motor(LEFT_EN_3_GPIO_Port, LEFT_EN_3_Pin, RIGHT_EN_3_GPIO_Port, RIGHT_EN_3_Pin, &htim1, TIM_CHANNEL_3),
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


const uint16_t logFormatId = 17;


const uint16_t sensorDataLength = sizeof(SensorData_t);

static_assert(sensorDataLength == 317,"aaaaaaa");


Lidar lidar = Lidar(&huart6);

const float ValveFitPressureRatios[4][11] = {{0, 0.08017, 0.18849, 0.27854, 0.37946, 0.48270, 0.52072, 0.58562, 0.61721, 0.61722, 0.63468},
		{0, 0.13364, 0.23557, 0.32940, 0.42822, 0.48331, 0.53583, 0.59307, 0.61894, 0.61990, 0.63189},
		{0, 0.10398, 0.24696, 0.32830, 0.44219, 0.48934, 0.55767, 0.60733, 0.63947, 0.64537, 0.65634},
		{0, 0.07359, 0.21091, 0.29041, 0.37703, 0.49174, 0.51417, 0.55939, 0.61397, 0.61397, 0.62187}};
//gnc::AltitudeEstimator altEstimator;
AltitudeEstimator::Params p;
AltitudeEstimator g_altEst;



static_assert(encoderSigns[0]==1 or encoderSigns[0]==-1,"degerler sadece +1 ve -1 olabilir.");
static_assert(encoderSigns[1]==1 or encoderSigns[1]==-1,"degerler sadece +1 ve -1 olabilir.");
static_assert(encoderSigns[2]==1 or encoderSigns[2]==-1,"degerler sadece +1 ve -1 olabilir.");
static_assert(encoderSigns[3]==1 or encoderSigns[3]==-1,"degerler sadece +1 ve -1 olabilir.");

PlatformController platform_controller;

const float nozzle_gain[4] = {1/0.1699, 1/0.1723, 1/0.1730, 1/0.1763};
