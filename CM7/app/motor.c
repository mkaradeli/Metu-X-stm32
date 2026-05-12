/*
 * motor.c
 *
 *  Created on: May 11, 2026
 *      Author: karadeli
 */



#include "motor.h"
#include <math.h>
#include <stdlib.h>   /* abs() */

/* ---- forward decls for the private helpers (were private methods) ---- */
static void Motor_SetDutyCycle(Motor* m, uint16_t duty_cycle);
static void Motor_SetDirectionClockWise(Motor* m);
static void Motor_SetDirectionCounterClockWise(Motor* m);
static void Motor_Brake(Motor* m);

/* ---- init (replaces constructor) ---- */
void Motor_Init(Motor* m,
                uint8_t id,
                bool magnetPolarity,
                GPIO_TypeDef* leftPort,  uint16_t leftPin,
                GPIO_TypeDef* rightPort, uint16_t rightPin,
                TIM_HandleTypeDef* pwmTimer, uint8_t pwmChannel)
{
    m->id             = id;
    m->magnetPolarity = magnetPolarity;
    m->leftPort       = leftPort;
    m->leftPin        = leftPin;
    m->rightPort      = rightPort;
    m->rightPin       = rightPin;
    m->pwmTimer       = pwmTimer;
    m->pwmChannel     = pwmChannel;

    m->dutyCycle      = 0.0f;
    m->dir            = MOTOR_DIR_BRAKE;
    m->current_bias   = 0.0f;
    m->current_meas   = 0.0f;
}

/* ---- current sensing ---- */
void Motor_InitCurrent(Motor* m, uint16_t* raw_value)
{
    (void)raw_value;  /* preserved from original: raw_value is not used */
    m->current_bias += m->current_meas;
}

void Motor_UpdateCurrent(Motor* m, uint16_t* raw_value)
{
    float mean = 0.0f;
    for (int i = 0; i < 32; i++) {
        mean += raw_value[i * 4];
    }
    mean /= 32.0f;
    /* float measured_voltage = raw_value/ADC_MAX * 3.3; */
    m->current_meas = -(mean / ADC_16B_MAX * 5.0f - 2.5f) / 0.066f - m->current_bias;
}

float Motor_GetCurrent(const Motor* m)
{
    return m->current_meas;
}

/* ---- direction control (private) ---- */
static void Motor_SetDirectionClockWise(Motor* m)
{
    HAL_GPIO_WritePin(m->leftPort,  m->leftPin,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(m->rightPort, m->rightPin, GPIO_PIN_RESET);
}

static void Motor_SetDirectionCounterClockWise(Motor* m)
{
    HAL_GPIO_WritePin(m->leftPort,  m->leftPin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(m->rightPort, m->rightPin, GPIO_PIN_SET);
}

static void Motor_Brake(Motor* m)
{
    HAL_GPIO_WritePin(m->leftPort,  m->leftPin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(m->rightPort, m->rightPin, GPIO_PIN_RESET);
}

/* ---- speed / duty cycle ---- */
void Motor_SetSpeed(Motor* m, float normalValue)
{
    if (normalValue > DUTY_DEAD_ZONE) {
        Motor_SetDirectionCounterClockWise(m);
        m->dir = MOTOR_DIR_COUNTERCLOCKWISE;
    } else if (normalValue < DUTY_DEAD_ZONE) {
        Motor_SetDirectionClockWise(m);
        m->dir = MOTOR_DIR_CLOCKWISE;
    } else {
        Motor_Brake(m);
        m->dir = MOTOR_DIR_BRAKE;
    }

    Motor_SetDutyCycle(m, (uint16_t)abs((int32_t)(normalValue * DUTY_MAX)));
    m->dutyCycle = normalValue;
}

float Motor_GetDutyCycle(const Motor* m)
{
    return m->dutyCycle;
}

static void Motor_SetDutyCycle(Motor* m, uint16_t duty_cycle)
{
    __HAL_TIM_SET_COMPARE(m->pwmTimer, m->pwmChannel, duty_cycle);
    m->dutyCycle = duty_cycle;
}
