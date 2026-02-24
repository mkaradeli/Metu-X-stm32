/*
 * Actuator.hpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#ifndef ACTUATOR_ACTUATOR_HPP_
#define ACTUATOR_ACTUATOR_HPP_

#include "Motor.hpp"
#include "PressureSensor.hpp"
#include "encoder.hpp"
#include "currentController.h"
#include "pressureController.h"
#include "positionController.h"

class Actuator {
public:
	Actuator(HallEffect* hallEffect,
		PressureSensor* psSensor,
		Motor* motor,
		Kalman* kalman);

	PressureSensor* psSensor;
	HallEffect* hallEffect;
	Kalman* motorKalman;
	Motor* motor;
	controller::pressure pressureController;
	controller::position positionController;
	controller::current currentController;
private:

//	CurrentController current_controller;

};




#endif /* ACTUATOR_ACTUATOR_HPP_ */
