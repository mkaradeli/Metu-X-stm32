/*
 * motor.h
 *
 *  Created on: May 11, 2026
 *      Author: karadeli
 */

#ifndef MOTOR_H_
#define MOTOR_H_


#include <stdint.h>
#include <stdbool.h>
//#include "globals.h"
//#include "gpio.h"
#include "stm32h7xx_hal.h"

#include "stm32h7xx_nucleo.h"

//#include "tim.h"

//#define ADC_MAX        4096U
#define ADC_16B_MAX    65536U
//#define HALF_ADC_MAX   (ADC_MAX / 2)
/* #define HALF_ADC_16B_MAX (ADC_16B_MAX / 2) */
#define DUTY_MAX       2499
/* #define DUTY_MAX     2047 */
#define DUTY_DEAD_ZONE 0

typedef enum {
    MOTOR_DIR_CLOCKWISE        = 1,
    MOTOR_DIR_COUNTERCLOCKWISE = 0,
    MOTOR_DIR_BRAKE            = 2
} MotorDirection;

typedef struct {
    uint8_t        id;
    float          dutyCycle;
    MotorDirection dir;

    GPIO_TypeDef*  leftPort;
    uint16_t       leftPin;

    GPIO_TypeDef*  rightPort;
    uint16_t       rightPin;

    TIM_HandleTypeDef* pwmTimer;
    uint8_t            pwmChannel;

    bool  magnetPolarity;
    float current_meas;
    float current_bias;
} Motor;

/* Replaces the C++ constructor. Initializes all fields. */
void  Motor_Init(Motor* m,
                 uint8_t id,
                 bool magnetPolarity,
                 GPIO_TypeDef* leftPort,  uint16_t leftPin,
                 GPIO_TypeDef* rightPort, uint16_t rightPin,
                 TIM_HandleTypeDef* pwmTimer, uint8_t pwmChannel);

void  Motor_InitCurrent(Motor* m, uint16_t* raw_value);
void  Motor_UpdateCurrent(Motor* m, uint16_t* raw_value);
float Motor_GetCurrent(const Motor* m);

void  Motor_SetSpeed(Motor* m, float normalValue);
float Motor_GetDutyCycle(const Motor* m);




#endif /* MOTOR_H_ */
