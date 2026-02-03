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
}

void PressureSensor::updatePS(uint16_t raw_value){
	const uint16_t y[13] = {0, 100, 300, 500, 1100, 1500, 2100, 2500, 3100, 3500, 4100, 4300, 4400};
    // Clamp to bounds
    if (raw_value <= calibration[0]) {
        bar = y[0];
    }
    if (raw_value >= calibration[NUM_POINTS - 1]) {
        bar = y[NUM_POINTS - 1];
    }

    // Find interval
    for (uint8_t i = 0; i < NUM_POINTS - 1; i++) {
        if (raw_value <= calibration[i + 1]) {
            float t = static_cast<float>(raw_value - calibration[i]) / static_cast<float>(calibration[i + 1] - calibration[i]);
            bar = y[i] + t * (y[i + 1] - y[i]);
            return;
        }
    }

    // Should never happen
    bar =  y[NUM_POINTS - 1];

//	bar = ((raw_value - Q0) * P1 + (Q1 - raw_value) * P0) / (Q1 - Q0);
}


//void PressureSensor::updatePS(uint16_t raw_value){
//	if (calibrateQ0) {
//		Q0 = raw_value;
//		calibrateQ0 = false;
//	}
//	if (calibrateQ1) {
//		Q1 = raw_value;
//		calibrateQ1 = false;
//	}
//
//	bar = ((raw_value - Q0) * P1 + (Q1 - raw_value) * P0) / (Q1 - Q0);
//}
//

