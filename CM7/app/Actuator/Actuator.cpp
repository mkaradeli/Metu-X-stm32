/*
 * Actuator.cpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#include "Actuator.hpp"
#include "actuatorController.h"
extern controller_modes controller_mode;


Actuator::Actuator(
		PressureSensor *psSensor_ptr,
//		PressureSensor *manifold_ptr,`
		uint16_t* encoder_adc_buffer,
		Motor* motor_ptr) {
	this->hallEffect.init(encoder_adc_buffer, &motorKalman);
	this->motor = motor_ptr;
	this->current.initialize();
	this->psSensor = psSensor_ptr;

}


void Actuator::updateCurrent(uint16_t raw_value) {
	this->current_meas = -((float) raw_value * 5.0f / ADC_16B_MAX - 2.5) / 0.066 - this->current_bias ;
}
float Actuator::get_current() {
	return this->current_meas;
}

void Actuator::current_controller_step() {
	this->current.rtU.current_feedback = this->current_meas;
	this->current.step();
	if (controller_mode>controller_modes::DISABLE)
		this->setDuty(this->current.rtY.Duty);
	else
		this->setDuty(0);
};

void Actuator::actuator_controller_step() {
	this->actuatorController.rtU.P_nozzle = this->getPressurePsi();
	this->actuatorController.rtU.P_manifold = Actuator::manifold->getPsi();
	this->actuatorController.rtU.SpeedFeedback = this->hallEffect.valveVelocity;
	this->actuatorController.rtU.pos_feedback = this->hallEffect.valveAngleKalman;
	this->actuatorController.step();
	this->current.rtU.current_ref = this->actuatorController.rtY.currentDemand;
}

void Actuator::updateHallEffect() {
	this->hallEffect.update_subBuffer();
}
void Actuator::setDuty(float normalValue) {
	this->motor->setDuty(normalValue);
}
float Actuator::getDutyCycle(){
	return motor->getDutyCycle();
}

void Actuator::calibrate(){
	this->updateHallEffect();
	this->hallEffect.calibrate();
	this->psSensor->calibrate();
	this->current_bias = this->current_meas;
}

void Actuator::readPressure(){
	this->psSensor->updatePS();
}
//void Actuator::readManifoldPressure(){
//	this->manifold->updatePS();
//}

float Actuator::getPressurePsi(){
	return this->psSensor->getPsi();
}
