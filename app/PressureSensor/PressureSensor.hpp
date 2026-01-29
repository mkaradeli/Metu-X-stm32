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
	PressureSensor(float P0, float P1, uint16_t Q0, uint16_t Q1);
	void updatePS(uint16_t raw_value);
	float getBar(){
		return this->bar;
	}
private:
//	uint8_t id;
	float P0;
	float P1;
	uint16_t Q0;
	uint16_t Q1;
	bool calibrateQ0;
	bool calibrateQ1;
	float bar;
};


#endif /* PRESSURESENSOR_PRESSURESENSOR_HPP_ */
