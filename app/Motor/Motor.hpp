/*
 * Motor.hpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */

#ifndef MOTOR_MOTOR_HPP_
#define MOTOR_MOTOR_HPP_

#include <stdint.h>
#include "gpio.h"
#include "tim.h"

#define ADC_MAX 4096
#define HALF_ADC_MAX ADC_MAX/2
#define DUTY_MAX 4095
#define DUTY_DEAD_ZONE 0

enum Direction {
    Clockwise = 1,
    CounterClockwise = 0,
	Brake = 2
};

class Motor{
public:
	Motor(uint8_t Id, bool MagnetPolarity, GPIO_TypeDef* LeftPort, uint16_t LeftPin, GPIO_TypeDef* RightPort, uint16_t RightPin, TIM_HandleTypeDef* PwmTimer, uint8_t PwmChannel);
	void updatePosition(uint16_t raw_value);
	void updateCurrent(uint16_t raw_value);
	float getPositionDegree();
	void setSpeed(float normalValue);
	void setPositionDegree(float value);
	float getDutyCycle();
	float getCurrent();
private:
	uint8_t id;
	float dutyCycle = 0;
	Direction dir = Direction::Brake;

    GPIO_TypeDef* leftPort;
    uint16_t leftPin;

    GPIO_TypeDef* rightPort;
    uint16_t rightPin;

    TIM_HandleTypeDef* pwmTimer;
    uint8_t pwmChannel;


	int32_t encoderDeltaValue = 0;
	uint16_t previousEncoderValue = 0;
	float positionDegree = 0;
	bool magnetPolarity = 0;
	float current_meas = 0;

	void setDutyCycle(uint16_t duty_cycle);
	void setDirectionClockWise();
	void setDirectionCounterClockWise();
	void motorBrake();
};







#endif /* MOTOR_MOTOR_HPP_ */
