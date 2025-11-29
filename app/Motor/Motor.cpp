/*
 * Motor.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */

#include "Motor.hpp"
#include <math.h>

Motor::Motor(uint8_t Id, bool MagnetPolarity, GPIO_TypeDef* LeftPort, uint16_t LeftPin, GPIO_TypeDef* RightPort, uint16_t RightPin, TIM_HandleTypeDef* PwmTimer, uint8_t PwmChannel){
	this->id = Id;
	this->magnetPolarity = MagnetPolarity;
	this->leftPort = LeftPort;
	this->leftPin = LeftPin;
	this->rightPort = RightPort;
	this->rightPin = RightPin;
	this->pwmTimer = PwmTimer;
	this->pwmChannel = PwmChannel;
}

void Motor::updatePosition(uint16_t raw_value){
	if(this->previousEncoderValue > 0)
		this->encoderDeltaValue = raw_value - this->previousEncoderValue;
	this->previousEncoderValue = raw_value;
	if (this->encoderDeltaValue > HALF_ADC_MAX){
		this->encoderDeltaValue -= ADC_MAX;
	}
	else if (this->encoderDeltaValue < -HALF_ADC_MAX){
		this->encoderDeltaValue += ADC_MAX;
	}

	if (this->magnetPolarity)
		this->positionDegree -= this->encoderDeltaValue*360.0f / ADC_MAX;
	else
		this->positionDegree += this->encoderDeltaValue*360.0f / ADC_MAX;
}

void Motor::updateCurrent(uint16_t raw_value){
//	float measured_voltage = raw_value/ADC_MAX * 3.3;
	this->current_meas = (raw_value*1.0f /ADC_MAX * 3.3f - 2.5) / 0.066 ;
}

float Motor::getPositionDegree(){
	return this->positionDegree;
}

float Motor::getCurrent(){
	return this->current_meas;
}

void Motor::setPositionDegree(float value){
	this->positionDegree = value;
}

void Motor::setDirectionClockWise(){
	HAL_GPIO_WritePin(this->leftPort, this->leftPin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(this->rightPort, this->rightPin, GPIO_PIN_RESET);
}

void Motor::setDirectionCounterClockWise(){
	HAL_GPIO_WritePin(this->leftPort, this->leftPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(this->rightPort, this->rightPin, GPIO_PIN_SET);
}

void Motor::motorBrake(){
	HAL_GPIO_WritePin(this->leftPort, this->leftPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(this->rightPort, this->rightPin, GPIO_PIN_RESET);
}

void Motor::setSpeed(float normalValue){
//	if ( (this->positionDegree > 1500.0) && (normalValue > 0.0))
//		normalValue = 0;
//	if ( (this->positionDegree < 15.0) && (normalValue < 0.0))
//		normalValue = 0;

	if (normalValue > DUTY_DEAD_ZONE){
		this->setDirectionCounterClockWise();
		this->dir = Direction::CounterClockwise;
	}
	else if (normalValue < DUTY_DEAD_ZONE){
		this->setDirectionClockWise();
		this->dir = Direction::Clockwise;
	}
	else
	{
		this->motorBrake();
		this->dir = Direction::Brake;
	}

	this->setDutyCycle(abs((int32_t)(normalValue*DUTY_MAX)));
	this->dutyCycle = normalValue;
}

float Motor::getDutyCycle(){
	return this->dutyCycle;
}

void Motor::setDutyCycle(uint16_t duty_cycle){
	__HAL_TIM_SET_COMPARE(this->pwmTimer, this->pwmChannel, duty_cycle);
	this->dutyCycle = duty_cycle;
}
