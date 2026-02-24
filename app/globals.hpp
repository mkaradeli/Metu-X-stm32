
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

#include "Lidar.hpp"
#include "Motor.hpp"
#include "PressureSensor.hpp"
#include "sh2_SensorValue.h"
#include "encoder.hpp"
#include "Actuator.hpp"

#include "currentController.h"
#include "positionController.h"
#include "pressureController.h"

#include "cmsis_os.h"





// Message Pack
typedef struct {
    uint32_t timestamp;
//    float motor_duty;
    float current_measured;
    float current_demand;
//    float encoderButt;
//    float vel_measured;
//    float motor_pos_kalman;
//    float angleRaw;
//    uint16_t current_raw;
    float valveAngle[4];
    float valveAngleKalman[4];
    float valveVelocity[4];
    float current_subsample[8];
    float duty_subsample[8];

    float speedDemand;
    float pos_ref;

    float pos_ref_rate_limited;
    float speed_ref_rate_limited;

    float manifold_pressure;
    float nozzle_pressure;

    float pressure_demand;





} SensorData_t;




// static_assert(sizeof(SensorData_t) == 48);
// static_assert(sizeof(SensorData_t) == 328);

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
extern const uint16_t sensorDataLength;

extern HallEffect hallEffect[4];
extern Kalman actuatorKalman[4];

extern controller::current current_controller[4];
extern controller::position position_controller[4];
//extern controller::pressure pressure_controller[4];

extern Actuator actuator[4];
extern bool file_creation_ok;


//extern int lastWriteDone;
#endif /* GLOBALS_HPP_ */
