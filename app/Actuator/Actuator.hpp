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
#include "actuatorController.h"
//#include "positionController.h"

class Actuator {
public:
	Actuator(uint16_t* encoder_adc_buffer,
		PressureSensor* psSensor,
		Motor* motor);

	uint16_t* encoder_adc_buffer;
	PressureSensor* psSensor;
	Motor* motor;
	HallEffect hallEffect;
	Kalman motorKalman;
	controller::current currentController;
	controller::actuatorController actuatorController;
//	controller::pressure pressureController;
	void updateCurrent(uint16_t* encoder_buffer);
	void current_controller_step();
	void actuator_controller_step();
//	void pressure_controller_step();
	void updateHallEffect();
	void calibrate();
private:


};




#endif /* ACTUATOR_ACTUATOR_HPP_ */
