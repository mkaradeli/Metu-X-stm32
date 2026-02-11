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



uint16_t psSensorsCalibration1[13] = {13000, 14293, 16685, 18902, 25937, 30628, 37523, 42237, 49214, 53846, 60952, 63122, 64406};
uint16_t psSensorsCalibration2[13] = {13000, 14360, 16673, 18998, 25832, 30482, 37281, 42001, 49137, 53694, 60819, 62981, 64330};

PressureSensor psSensors[MAX_PS_COUNT] = {
		PressureSensor(psSensorsCalibration1),
		PressureSensor(psSensorsCalibration2),
		PressureSensor(psSensorsCalibration2),
		PressureSensor(psSensorsCalibration2),
		PressureSensor(psSensorsCalibration2),
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

CurrentController current_controller[4];
positionController position_controller[4];

Actuator actuator[4] = {
	Actuator(&hallEffect[0], &psSensors[0], &motors[0], &actuatorKalman[0]),
	Actuator(&hallEffect[1], &psSensors[1], &motors[1], &actuatorKalman[1]),
	Actuator(&hallEffect[2], &psSensors[2], &motors[2], &actuatorKalman[2]),
	Actuator(&hallEffect[3], &psSensors[3], &motors[3], &actuatorKalman[3]),
};



const uint16_t logFormatID = 8;
const uint16_t* logFormatID_ptr = &logFormatID;
const uint16_t sensorDataLength = sizeof(SensorData_t);



// SD Card Variables

TaskHandle_t sdCardTaskHandle;
//int lastWriteDone = 0;
// encoder Variables;
//bool file_creation_ok = false;
int left_filename_index = 0;
int right_filename_index = 0;
//encoder_ss encoder[4];


