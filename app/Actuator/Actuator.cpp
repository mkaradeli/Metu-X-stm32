/*
 * Actuator.cpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#include "Actuator.hpp"


Actuator::Actuator(HallEffect* hallEffect,
	PressureSensor* psSensor,
	Motor* motor,
	Kalman* kalman) {

	this->hallEffect = hallEffect;
	this-> psSensor = psSensor;
	this->motor = motor;
	this->motorKalman = kalman;
	pressureController.initialize();
	positionController.initialize();
	currentController.initialize();

}
