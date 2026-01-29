/*
 * PressureSensor.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */
#include "PressureSensor.hpp"

PressureSensor::PressureSensor(float P0, float P1, uint16_t Q0, uint16_t Q1){
	this->P0 = P0;
	this->P1 = P1;
	this->Q0 = Q0;
	this->Q1 = Q1;
}

void PressureSensor::updatePS(uint16_t raw_value){
	if (calibrateQ0) {
		Q0 = raw_value;
		calibrateQ0 = false;
	}
	if (calibrateQ1) {
		Q1 = raw_value;
		calibrateQ1 = false;
	}

	bar = ((raw_value - Q0) * P1 + (Q1 - raw_value) * P0) / (Q1 - Q0);
}


