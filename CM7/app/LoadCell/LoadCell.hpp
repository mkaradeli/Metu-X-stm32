/*
 * LoadCell.hpp
 *
 *  Created on: Jun 27, 2026
 *      Author: karadeli
 */

#ifndef LOADCELL_LOADCELL_HPP_
#define LOADCELL_LOADCELL_HPP_
#include "LowPassFilter.hpp"
#include <stdint.h>


class LoadCell {
public:
	LoadCell(uint16_t* raw_value);
	float update();
	float getForce();
	float weight_N = 0;
	float weight_kg = 0;
	float weight_kg_filtered = 0;
	float weight_N_filtered = 0;
	uint16_t* raw_value;
private:
	LowPass lowPass{1,1000, 1};
	float current;
	volatile float curMin = 4;
	volatile float curMax = 20;
	volatile float weightMin = 0;
	volatile float weightMax = 100;

};


#endif /* LOADCELL_LOADCELL_HPP_ */
