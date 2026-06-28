/*
 * PressureSensor.hpp
 *
 *  Created on: Aug 8, 2025
 *      Author: karadeli
 */

#ifndef PRESSURESENSOR_PRESSURESENSOR_HPP_
#define PRESSURESENSOR_PRESSURESENSOR_HPP_

#include <stdint.h>
#include "LowPassFilter.hpp"

class PressureSensor {
public:
	PressureSensor(uint16_t *raw_value);
	void calibrate();
	void updatePS();
	float getBar(){
		return this->bar;
	}
	float getPsi(){
		return this->psi;
	}

	float current;
	LowPass lowPass{1.0f, 1000.0f};
//	LowPass load_lpf{0.1f, 66.6};  // 30 Hz cutoff @ 1 kHz sample rate

private:
	float bar;
	float bar_filtered;
	float psi;
	uint16_t * raw_value;
};


#endif /* PRESSURESENSOR_PRESSURESENSOR_HPP_ */
