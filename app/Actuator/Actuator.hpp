/*
 * Actuator.hpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#ifndef ACTUATOR_ACTUATOR_HPP_
#define ACTUATOR_ACTUATOR_HPP_

#include "Motor.hpp"
#include "encoder.hpp"
#include "PressureSensor.hpp"
#include "currentController.h"
#include "actuatorController.h"
#include "actuatorKalman.h"

class Actuator {
public:
	Actuator(PressureSensor *psSensor_ptr, uint16_t* encoder_adc_buffer, Motor* motor_ptr);
	void updateCurrent(uint16_t raw_value);
	float get_current();
	void current_controller_step();
	void actuator_controller_step();
//	void pressure_controller_step();
	void updateHallEffect();
	void calibrate();
	void setDuty(float normalValue);
	float getDutyCycle();
	void readPressure();
	void readManifoldPressure();
	float getPressurePsi();
//	void calibrate();

//	void measure_current(uint16_t raw_value);


	controller::actuatorController actuatorController;
	HallEffect hallEffect;
	static PressureSensor *manifold;
	PressureSensor *psSensor;

private:
	float current_meas;
	float current_bias;

	controller::current current;
	Kalman motorKalman;

	Motor* motor;



};




#endif /* ACTUATOR_ACTUATOR_HPP_ */
