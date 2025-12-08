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

#define ADC2_BUF_LEN     24
#define ADC2_CH_COUNT    4

#include <stdint.h>

#include "Lidar.hpp"
#include "Motor.hpp"
#include "PressureSensor.hpp"
#include "sh2_SensorValue.h"

#include "cmsis_os.h"





// Message Pack
typedef struct {
    uint32_t timestamp;
    float motor_duty;
    float current_measured;
    float current_demand;
    float encoderFront;
    float encoderButt;
    float encoderGeared;
    float vel_measured;
    float vel_demand;
    float motor_pos_kalman;
    float motor_pos_demand;
    float mass_estimation;
    float pressure_manifold;
    float pressure_nozzle;
    float pressure_demand;
    float force_feedback;
    float force_measured;
    float thrust_demand;
    uint16_t encoder_readings[ADC2_BUF_LEN];
    uint16_t current_readings[ADC2_BUF_LEN];

} SensorData_t;

//static_assert(sizeof(SensorData_t) == 328);

typedef union {
    SensorData_t data;
    uint8_t bytes[sizeof(SensorData_t)];
} SensorDataUnion_t;

extern SensorDataUnion_t txData;

// Lidar Variables
extern TaskHandle_t lidarTaskHandle;
extern Lidar lidar;

// Pressure Sensor Variables
extern TaskHandle_t psTaskHandle;
extern uint16_t PSValues[MAX_PS_COUNT];
extern PressureSensor psSensors[MAX_PS_COUNT];

// Motor Variables
extern TaskHandle_t motorTaskHandle;
extern uint16_t EncoderValues[ADC2_CH_COUNT*ADC2_BUF_LEN];
extern Motor motors[MAX_MOTOR_COUNT];

// SD Card Varibles
extern TaskHandle_t sdCardTaskHandle;

//extern const uint8_t logHeader[];
extern const char* logHeader_ptr;
extern const uint8_t logHeaderSize;
//extern const uint16_t logFormatID;
extern const uint16_t* logFormatID_ptr;




#endif /* GLOBALS_HPP_ */
