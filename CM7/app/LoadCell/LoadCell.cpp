/*
 * LoadCell.cpp
 *
 *  Created on: Jun 27, 2026
 *      Author: karadeli
 */
#include "LoadCell.hpp"

LoadCell::LoadCell(uint16_t* raw_value) {
	weight_N = 0;
	weight_kg = 0;
	current = 0;
	this->raw_value = raw_value;
}

float LoadCell::update() {
	current = static_cast<float>(*raw_value) * 5.0f/65536.0f / 140.0f*1000.0f; // mA
	weight_kg = ((current -curMin)*weightMax + (curMax-current)*weightMin)/ (curMax-curMin);
	weight_N = weight_kg * 9.8065;
	weight_kg_filtered = lowPass.update(weight_kg);
	weight_N_filtered =  weight_kg_filtered * 9.8065;
	return weight_N;
}
float LoadCell::getForce() {
	return weight_kg;
}

//public:
//	LoadCell();
//	float update();
//	float getWeight();
//private:
//	LowPass lowPass{1,1000};
//	float curMin = 4;
//	float curMax = 100;
//	float weightMin = 0;
//	float weightMax = 100;
