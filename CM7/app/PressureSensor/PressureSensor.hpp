/*
 * PressureSensor.hpp
 *
 *  Created on: Aug 8, 2025
 *      Author: alica
 */

#ifndef PRESSURESENSOR_PRESSURESENSOR_HPP_
#define PRESSURESENSOR_PRESSURESENSOR_HPP_

#include <stdint.h>

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
	float voltage;
private:
//	uint8_t id;
	uint16_t * raw_value;
//	uint16_t calibration[2];
	float psi_shift;
	float bar;
	float psi;
};


#endif /* PRESSURESENSOR_PRESSURESENSOR_HPP_ */
