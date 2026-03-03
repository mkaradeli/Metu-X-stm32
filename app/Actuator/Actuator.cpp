/*
 * Actuator.cpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#include "Actuator.hpp"


Actuator::Actuator(uint16_t* encoder_adc_buffer,
	PressureSensor* psSensor,
	Motor* motor) {

//	this->hallEffect = hallEffect;
	this->hallEffect.init(encoder_adc_buffer, &motorKalman);
	this->encoder_adc_buffer = encoder_adc_buffer;
	this-> psSensor = psSensor;
	this->motor = motor;
//	this->motorKalman = kalman;
	pressureController.initialize();
	positionController.initialize();
	currentController.initialize();

}


void Actuator::updateCurrent(uint16_t* encoder_buffer) {
	this->motor->updateCurrent(encoder_buffer);
}
void Actuator::current_controller_step() {
//	this->motor->updateCurrent(this->hallEffect->adc_buffer);
	this->currentController.rtU.current_feedback = this->motor->getCurrent();
	this->currentController.step();
	this->motor->setSpeed(this->currentController.rtY.Duty);
};

void Actuator::position_controller_step() {
	this->positionController.rtU.SpeedFeedback = this->hallEffect.valveVelocity;
	this->positionController.rtU.pos_feedback = this->hallEffect.valveAngleKalman;
	this->positionController.step();
	this->currentController.rtU.current_ref = this->positionController.rtY.currentDemand;
}

void Actuator::pressure_controller_step() {
	this->pressureController.step();
	this->positionController.rtU.pos_ref = this->pressureController.rtY.position_demand;
	this->position_controller_step();
}

void Actuator::updateHallEffect() {
	this->hallEffect.update_subBuffer();
}
