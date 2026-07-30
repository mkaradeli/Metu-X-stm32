/*
 * PressureSensor.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */
#include "PressureSensor.hpp"

#define NUM_POINTS 13

PressureSensor::PressureSensor(uint16_t *raw_value){
//	for (int i = 0; i<13; i++)
//		this->calibration[i] = calibration[i];
	this->raw_value = raw_value;
//	this->psi_shift = 0;
}

void PressureSensor::calibrate(){

}
void PressureSensor::updatePS(){
	current = static_cast<float>(*raw_value) * 3.3f/65536.0f / 140.0f*1000.0f; // mA
	bar = (current - 4.0f)/16.0f *315.0f;
	bar_filtered = lowPass.update(bar);
	psi = bar / 0.0689475729;
}


