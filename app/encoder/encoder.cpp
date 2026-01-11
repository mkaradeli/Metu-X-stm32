/*
 * encoder.cpp
 *
 *  Created on: Jan 11, 2026
 *      Author: karadeli
 */

#include "encoder.hpp"
#include <stdint.h>

HallEffect::HallEffect(uint16_t* adc_buffer) {
	this->wasInMiddle = false;
	this->shifter = 0;
	this->lastReading = 0;
	this->angleRaw = 0;
	this->angleFiltered = 0;
	this->velocity = 0;
	this->calibration = 0;
	this->adc_buffer = adc_buffer;
}

void HallEffect.calibrate() {
	this->wasInMiddle = (this->adc_buffer[0] < 49152) && (this->adc_buffer[0] > 16384);
	this->shifter = 0;
	this->lastReading = this->adc_buffer[0];
	this->angleRaw = this->adc_buffer[0] * (360.0f / ADC_16B_MAX);
	this->angleFiltered = 0;
	this->velocity = 0;
	float mean = 0;
	for (int i=0;i<SUBSAMPLE_COUNT;i++){
		mean += this->adc_buffer[i*4];
	}
	mean /= 64.0f;
	this->calibration = mean;
}

void HallEffect.update() {

	int8_t middle_counter = 0;
	for (int i = 0; i< SUBSAMPLE_COUNT; i++){
		(this->adc_buffer[i*4]<49152U) && (this->adc_buffer[i*4]>16384U) ? middle_counter++: middle_counter--;
	}
	bool inMiddle = middle_counter > 0;
	float mean = 0;


	if (inMiddle) {
		for (int i=0;i<SUBSAMPLE_COUNT;i++){
				mean += this->adc_buffer[i*4];
		}
		mean /= 64.0f;
		if (!this->wasInMiddle){
			(mean < 32768) ? this->shifter++ : this->shifter--;
		}
	}
	else {
		uint16_t temp;
//		double mean_shifted = 0;≤÷

		for (int i=0; i<SUBSAMPLE_COUNT;i++){
			temp = (this->adc_buffer[i*4]+32768)%65536;
			mean += temp;
		}
		mean /= 64.0f;
		if (this->wasInMiddle) {
			(mean < 32768) ? this->shifter++ : this->shifter--;
		}

	}
	this->lastReading = this->adc_buffer[0];
	this->wasInMiddle = inMiddle;
	this->angleRaw = (mean - this->calibration + this->shifter* 32768) * 360.0f / 65536;


}