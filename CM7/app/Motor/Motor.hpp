/*
 * Motor.hpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */

#ifndef MOTOR_MOTOR_HPP_
#define MOTOR_MOTOR_HPP_

#include <stdint.h>
#include <stdbool.h>
#include "stm32h7xx_hal.h"

#define ADC_16B_MAX    65536U
#define DUTY_MAX       2499
#define DUTY_DEAD_ZONE 0

enum Direction {
    Clockwise = 1,
    CounterClockwise = 0,
	Brake = 2
};


class Motor{
public:
	Motor(
//			uint8_t Id,
//			bool MagnetPolarity,
			GPIO_TypeDef* LeftPort,
			uint16_t LeftPin,
			GPIO_TypeDef* RightPort,
			uint16_t RightPin,
			TIM_HandleTypeDef* PwmTimer,
			uint8_t PwmChannel);

	void setDuty(float normalValue);
	float getDutyCycle();

private:
//	uint8_t id;
	float dutyCycle = 0;
	Direction dir = Direction::Brake;

    GPIO_TypeDef* leftPort;
    uint16_t leftPin;

    GPIO_TypeDef* rightPort;
    uint16_t rightPin;

    TIM_HandleTypeDef* pwmTimer;
    uint8_t pwmChannel;

//	bool magnetPolarity = 0;
	uint32_t CCR = 0;


	void setDutyCycle(uint16_t duty_cycle);
	void setDirectionClockWise();
	void setDirectionCounterClockWise();
	void motorBrake();
};


#endif /* MOTOR_MOTOR_HPP_ */
