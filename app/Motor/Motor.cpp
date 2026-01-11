/*
 * Motor.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */

#include "Motor.hpp"
#include <math.h>

Motor::Motor(uint8_t Id, bool MagnetPolarity, GPIO_TypeDef* LeftPort, uint16_t LeftPin, GPIO_TypeDef* RightPort, uint16_t RightPin, TIM_HandleTypeDef* PwmTimer, uint8_t PwmChannel, uint16_t* EncoderValuesAddress){
	this->id = Id;
	this->magnetPolarity = MagnetPolarity;
	this->leftPort = LeftPort;
	this->leftPin = LeftPin;
	this->rightPort = RightPort;
	this->rightPin = RightPin;
	this->pwmTimer = PwmTimer;
	this->pwmChannel = PwmChannel;
	this->EncoderValuesAddress = EncoderValuesAddress /*48 lik bir array*/;
	this->encoderInit();
}

void Motor::encoderInit( /*48 lik bir array*/) {

	this->encoder.wasInMiddle = (this->EncoderValuesAddress[0] < 49152) && (this->EncoderValuesAddress[0] > 16384);
	this->encoder.shifter = 0;
	this->encoder.lastReading = this->EncoderValuesAddress[0];
	this->encoder.angleRaw = this->EncoderValuesAddress[0] * (360.0f / 65536);
	this->encoder.angleFiltered = 0;
	this->encoder.velocity = 0;
	double mean = 0;
	for (int i=0;i<SUBSAMPLE_COUNT;i++){
		mean += this->EncoderValuesAddress[i*4];
	}
	mean /= 12.0f;
	this->encoder.calibration = mean;
};

void Motor::encoderUpdate() {
	int8_t middle_counter = 0;
	for (int i = 0; i< SUBSAMPLE_COUNT; i++){
		(this->EncoderValuesAddress[i*4]<49152U) && (this->EncoderValuesAddress[i*4]>16384U) ? middle_counter++: middle_counter--;
	}
	bool inMiddle = middle_counter > 0;
	double mean = 0;


	if (inMiddle) {
		for (int i=0;i<SUBSAMPLE_COUNT;i++){
				mean += this->EncoderValuesAddress[i*4];
		}
		mean /= 12.0f;
		if (!this->encoder.wasInMiddle){
			(mean < HALF_ADC_16B_MAX) ? this->encoder.shifter++ : this->encoder.shifter--;
		}
	}
	else {
		uint16_t temp;
//		double mean_shifted = 0;≤÷

		for (int i=0; i<SUBSAMPLE_COUNT;i++){
			temp = (this->EncoderValuesAddress[i*4]+HALF_ADC_16B_MAX)%ADC_16B_MAX;
			mean += temp;
		}
		mean /= 12.0f;
		if (this->encoder.wasInMiddle) {
			(mean < HALF_ADC_16B_MAX) ? this->encoder.shifter++ : this->encoder.shifter--;
		}

	}
	this->encoder.lastReading = this->EncoderValuesAddress[0];
	this->encoder.wasInMiddle = inMiddle;
	this->encoder.angleRaw = (mean - this->encoder.calibration + this->encoder.shifter* HALF_ADC_16B_MAX) * 360.0f / ADC_16B_MAX;

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

void Motor::initCurrent(uint16_t* raw_value){
	this->updateCurrent(raw_value);
	this->current_bias = this->current_meas;
}
void Motor::updateCurrent(uint16_t* raw_value){
	float mean = 0;
	for (int i=0;i<SUBSAMPLE_COUNT;i++){
			mean += raw_value[i*4];
	}
	mean /= 12.0f;
	//	float measured_voltage = raw_value/ADC_MAX * 3.3;
	this->current_meas = (mean /ADC_16B_MAX * 3.3f - 2.5) / 0.066 - this->current_bias ;
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
