/*
 * globals.cpp
 *
 *  Created on: Aug 17, 2025
 *      Author: alica
 */

#include "globals.hpp"



// Message Pack

SensorDataUnion_t txData;



// Lidar Variables

TaskHandle_t lidarTaskHandle;
Lidar lidar = Lidar(&huart2);



// Pressure Sensor Variables

TaskHandle_t psTaskHandle;
uint16_t PSValues[MAX_PS_COUNT] = {0};
PressureSensor psSensors[MAX_PS_COUNT] = {
		PressureSensor(1, 168.7),
		PressureSensor(2, 165.3),
		PressureSensor(3, 165.7),
		PressureSensor(4, 169.0),
		PressureSensor(5, 162.5)
};



// Motor Variables

TaskHandle_t motorTaskHandle;
uint16_t EncoderValues[ADC2_BUF_LEN*ADC2_CH_COUNT] = {0};
//uint16_t* EncoderValuesAddress = &EncoderValues[0];

Motor motors[MAX_MOTOR_COUNT] = {
		Motor(1, true, LEFT_EN_1_GPIO_Port, LEFT_EN_1_Pin, RIGHT_EN_1_GPIO_Port, RIGHT_EN_1_Pin, &htim4, TIM_CHANNEL_1),
		Motor(2, true, LEFT_EN_2_GPIO_Port, LEFT_EN_2_Pin, RIGHT_EN_2_GPIO_Port, RIGHT_EN_2_Pin, &htim4, TIM_CHANNEL_2),
		Motor(3, true, LEFT_EN_3_GPIO_Port, LEFT_EN_3_Pin, RIGHT_EN_3_GPIO_Port, RIGHT_EN_3_Pin, &htim4, TIM_CHANNEL_3),
		Motor(4, true, LEFT_EN_4_GPIO_Port, LEFT_EN_4_Pin, RIGHT_EN_4_GPIO_Port, RIGHT_EN_4_Pin, &htim4, TIM_CHANNEL_4)
};

Kalman actuatorKalman[4];

HallEffect hallEffect[4] = {
		HallEffect(&EncoderValues[0], &actuatorKalman[0]),
		HallEffect(&EncoderValues[1], &actuatorKalman[1]),
		HallEffect(&EncoderValues[2], &actuatorKalman[2]),
		HallEffect(&EncoderValues[3], &actuatorKalman[3]),
};

CurrentController current_controller;
positionController position_controller;

Actuator actuator[4] = {
	Actuator(&hallEffect[0], &psSensors[0], &motors[0], &actuatorKalman[0]),
	Actuator(&hallEffect[1], &psSensors[1], &motors[1], &actuatorKalman[1]),
	Actuator(&hallEffect[2], &psSensors[2], &motors[2], &actuatorKalman[2]),
	Actuator(&hallEffect[3], &psSensors[3], &motors[3], &actuatorKalman[3]),
};


const char logHeader[] = "position controller eklendi.";
const char* logHeader_ptr = &logHeader[0];
const uint8_t logHeaderSize = sizeof(logHeader);
const uint16_t logFormatID = 6;
const uint16_t* logFormatID_ptr = &logFormatID;
const uint16_t sensorDataLength = sizeof(SensorData_t);



// SD Card Variables

TaskHandle_t sdCardTaskHandle;
//int lastWriteDone = 0;
// encoder Variables;

//encoder_ss encoder[4];


