/*
 * Motor.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */

#include <math.h>
#include "Motor.hpp"

Motor::Motor(
//		uint8_t Id,
//		bool MagnetPolarity,
		GPIO_TypeDef* LeftPort,
		uint16_t LeftPin,
		GPIO_TypeDef* RightPort,
		uint16_t RightPin,
		TIM_HandleTypeDef* PwmTimer,
		uint8_t PwmChannel){
//	this->id = Id;
//	this->magnetPolarity = MagnetPolarity;
	this->leftPort = LeftPort;
	this->leftPin = LeftPin;
	this->rightPort = RightPort;
	this->rightPin = RightPin;
	this->pwmTimer = PwmTimer;
	this->pwmChannel = PwmChannel;

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

void Motor::setDuty(float normalValue){

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

	this->dutyCycle = normalValue;
	this->CCR = abs(normalValue*DUTY_MAX);
	__HAL_TIM_SET_COMPARE(this->pwmTimer, this->pwmChannel, this->CCR);


}

float Motor::getDutyCycle(){
	return this->dutyCycle;
}

