/*
 * PressureSensor.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */
#include "PressureSensor.hpp"

PressureSensor::PressureSensor(){
	P0 = 0;
	P1 = 300;
	Q0 = 0;
	Q1 = 4096;
	calibrateQ0 = false;
	calibrateQ1 = false;
}

void PressureSensor::updatePS(uint16_t raw_value){
	if (calibrateQ0) {
		Q0 = raw_value;
		calibrateQ0 = false;
	}
	else if (calibrateQ1) {
		Q1 = raw_value;
		calibrateQ1 = false;
	}

	bar = ((raw_value - Q0) * P1 + (Q1 - raw_value) * P0) / (Q1 - Q0);

}
