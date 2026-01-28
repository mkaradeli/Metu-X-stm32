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
private:

//	CurrentController current_controller;

};




#endif /* ACTUATOR_ACTUATOR_HPP_ */
