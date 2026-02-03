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
	PressureSensor(uint16_t calibration[13]);
	void updatePS(uint16_t raw_value);
	float getBar(){
		return this->bar;
	}
private:
//	uint8_t id;
	uint16_t calibration[13];
	float bar;
};


#endif /* PRESSURESENSOR_PRESSURESENSOR_HPP_ */
