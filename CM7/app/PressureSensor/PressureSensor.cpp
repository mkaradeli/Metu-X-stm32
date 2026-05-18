/*
 * PressureSensor.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */
#include "PressureSensor.hpp"

#define NUM_POINTS 13

PressureSensor::PressureSensor(uint16_t calibration[12]){
	for (int i = 0; i<13; i++)
		this->calibration[i] = calibration[i];
	this->psi_shift = 0;
}

void PressureSensor::calibrate(){
	this->psi_shift = this->psi;
}
void PressureSensor::updatePS(uint16_t raw_value){
	const uint16_t y[13] = {0, 100, 300, 500, 1100, 1500, 2100, 2500, 3100, 3500, 4100, 4300, 4400};
    // Clamp to bounds
    if (raw_value <= calibration[0]) {
        psi = y[0];
    }
    if (raw_value >= calibration[NUM_POINTS - 1]) {
        psi = y[NUM_POINTS - 1];
    }

    // Find interval
    for (uint8_t i = 0; i < NUM_POINTS - 1; i++) {
        if (raw_value <= calibration[i + 1]) {
            float t = static_cast<float>(raw_value - calibration[i]) / static_cast<float>(calibration[i + 1] - calibration[i]);
            psi = y[i] + t * (y[i + 1] - y[i]);
            break;
        }
    }

    psi = psi - psi_shift;

    bar = psi * 0.0689475729;
}


