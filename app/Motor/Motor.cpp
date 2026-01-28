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
	current_bias = 0;
	current_meas = 0;
}



void Motor::initCurrent(uint16_t* raw_value){
	current_bias += current_meas;
}
void Motor::updateCurrent(uint16_t* raw_value){
	float mean = 0;
	for (int i=0;i<32;i++){
			mean += raw_value[i*4];
	}
	mean /= 32.0f;
	//	float measured_voltage = raw_value/ADC_MAX * 3.3;
	this->current_meas = -(mean /ADC_16B_MAX * 3.3f - 2.5) / 0.066 - this->current_bias ;
}

float Motor::getCurrent(){
	return this->current_meas;
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
