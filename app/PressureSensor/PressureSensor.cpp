/*
 * PressureSensor.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */
#include "PressureSensor.hpp"

PressureSensor::PressureSensor(uint8_t Id, float CalibrationFactor){
	this->id = Id;
	this->calibrationFactor = CalibrationFactor;
}

void PressureSensor::updatePS(uint16_t raw_value){
	this->bar = ((3300*raw_value/4096/this->calibrationFactor)-4)*315/16;
}
