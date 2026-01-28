/*
 * encoder.cpp
 *
 *  Created on: Jan 11, 2026
 *      Author: karadeli
 */

#include "encoder.hpp"
#include "globals.hpp"
#include "rtwtypes.h"
#include "tim.h"
#include <stdint.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

HallEffect::HallEffect(uint16_t* adc_buffer, Kalman* actuatorKalman) {
	this->adc_buffer = adc_buffer;
	this->shifter = 0;
	this->shifter_subBuffer = 0;
	// this->lastReading = this->adc_buffer[0];
	this->angleRaw = 0;
	this->angle_Filtered = 0;
	this->velocity = 0;
	calibration = 0;
//	calibration = -lastReading;

	// this->calibrate();
	this->kalman = actuatorKalman;
	this->kalman->initialize();
	this->unstableCount = 0;


};

void HallEffect::calibrate() {
	calibration = -motorAngle;
	return;
	// this->wasInMiddle = (this->adc_buffer[0] < 49152) && (this->adc_buffer[0] > 16384);
	this->shifter = 0;
	this->shifter_subBuffer = 0;
	// this->lastReading = this->adc_buffer[0];
	this->angleRaw = 0;
	this->angle_Filtered = 0;
	this->velocity = 0;


	const uint16_t tickLimitForStability = 100;
	const uint16_t numberForStability = 4;
	// uint16_t subBuffer[numberForStability];

	uint16_t* adc_window_ptr = this->adc_buffer + (ADC2_BUF_LEN - numberForStability) * ADC2_CH_COUNT;
	uint16_t max_val = *adc_window_ptr, min_val = *adc_window_ptr;

	// bu döngü sensörün stabil olduğu n değeri bulmak için bufferı sondan başa tarayarak açıklık değeri tickLimitin altında kalan bölgeyi bulur.
	for (uint16_t windowIndex = 0; windowIndex < ADC2_BUF_LEN / numberForStability; windowIndex ++) {
		for (uint16_t inWindowIndex = 0; inWindowIndex < numberForStability; inWindowIndex++){
			max_val = MAX(max_val, adc_window_ptr[inWindowIndex*ADC2_CH_COUNT]);
			min_val = MIN(min_val, adc_window_ptr[inWindowIndex*ADC2_CH_COUNT]);
		}

		if ( (max_val - min_val) < tickLimitForStability) {
			velocity = windowIndex;
			break;
		}
		else {
			adc_window_ptr -= ADC2_CH_COUNT * numberForStability;
		}
	}

	uint32_t mean = 0;
	for (int index = 0; index < numberForStability; index++) {
		mean += adc_window_ptr[index*ADC2_CH_COUNT];
	}
	mean /= numberForStability;

	bool inMiddle;
	if (mean < 49152U and mean > 16384U) {
		inMiddle = true;
	}
	else {
		inMiddle = false;
		mean += HALF_ADC_16B_MAX;
		mean = mean % ADC_16B_MAX;
	}




	this->lastReading = this->adc_buffer[0];
	this->wasInMiddle = inMiddle;
	wasInMiddle_subBuffer = inMiddle;


	// float mean = 0;
	// for (int i=0;i<SUBSAMPLE_COUNT;i++){
	// 	mean += this->adc_buffer[i*4];
	// }
	// mean /= 64.0f;
	this->calibration = angleRaw;
};

void HallEffect::update() {
	int8_t middle_counter = 0;
	for (int i = 0; i< SUBSAMPLE_COUNT; i++){
		// adc_buffer[i*4] = 32000;
		(this->adc_buffer[i*4]<49152U) && (this->adc_buffer[i*4]>16384U) ? middle_counter++: middle_counter--;
	}
	bool inMiddle = middle_counter > 0;
	float mean = 0;


	if (inMiddle) {
		for (int i=0;i<SUBSAMPLE_COUNT;i++){
				// adc_buffer[i*4] = 32000;
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
	this->angleRaw = (mean + this->shifter* 32768) * 360.0f / 65536 - calibration;


};

void HallEffect::update_subBuffer() {
	uint8_t localUnstability = 0;
	const uint16_t tickLimitForStability = 100*16;
	const uint16_t numberForStability = 4;
	// uint16_t subBuffer[numberForStability];

	uint16_t* adc_window_ptr = this->adc_buffer + (ADC2_BUF_LEN - numberForStability) * ADC2_CH_COUNT;
	uint16_t max_val = *adc_window_ptr, min_val = *adc_window_ptr;

	// bu döngü sensörün stabil olduğu n değeri bulmak için bufferı sondan başa tarayarak açıklık değeri tickLimitin altında kalan bölgeyi bulur.
	for (uint16_t windowIndex = 0; windowIndex < ADC2_BUF_LEN / numberForStability - 1; windowIndex ++) {
		max_val = *adc_window_ptr;
		min_val = *adc_window_ptr;
		for (uint16_t inWindowIndex = 0; inWindowIndex < numberForStability; inWindowIndex++){
			max_val = MAX(max_val, adc_window_ptr[inWindowIndex*ADC2_CH_COUNT]);
			min_val = MIN(min_val, adc_window_ptr[inWindowIndex*ADC2_CH_COUNT]);
		}

		if ( (max_val - min_val) < tickLimitForStability) {
			// velocity = windowIndex;
			break;
		}
		else {
			localUnstability++;
			adc_window_ptr -= ADC2_CH_COUNT * numberForStability;
		}
	}

	if (localUnstability) {
		unstableCount = localUnstability;
	}

	uint32_t mean = 0;
	for (int index = 0; index < numberForStability; index++) {
		mean += adc_window_ptr[index*ADC2_CH_COUNT];
	}
	mean /= numberForStability;

	bool inMiddle;

//	int8_t middle_counter = 0;

//	for (int i = 0; i< numberForStability; i++){
//		(adc_window_ptr[i* ADC2_CH_COUNT] < 49152U && adc_window_ptr[i* ADC2_CH_COUNT] > 16384U) ? middle_counter++ : middle_counter--;
//	}
//	if (middle_counter>=0){
	 if (mean < 49152U and mean > 16384U) {
		inMiddle = true;
	}
	else {
		inMiddle = false;
	}

	if (inMiddle) {
		if (not this->wasInMiddle_subBuffer) {
			if (mean < HALF_ADC_16B_MAX) {
				this->shifter_subBuffer++;
				// shifter += 1;
			}
			else {
				this->shifter_subBuffer--;
				// shifter -= 1;
			}
		}
	}
	else {
//		mean = 0;
//		for (int index = 0; index < numberForStability; index++) {
//			mean += (adc_window_ptr[index*ADC2_CH_COUNT] + HALF_ADC_16B_MAX) % ADC_16B_MAX;
//		}
//		mean /= numberForStability;

		mean += HALF_ADC_16B_MAX;
		mean = mean % ADC_16B_MAX;
		if (wasInMiddle_subBuffer) {
			if (mean < HALF_ADC_16B_MAX){
				this->shifter_subBuffer++;
				// shifter++;
			}
			else {
				this->shifter_subBuffer--;
				// shifter--;
			}
		}
	}

	mean_subBuffer = mean;
	this->wasInMiddle_subBuffer = inMiddle;

	this->motorAngle = (static_cast<double>(mean) + static_cast<double>(this->shifter_subBuffer) * HALF_ADC_16B_MAX) / ADC_16B_MAX * 360.0f + calibration;
	this->valveAngle = this->motorAngle / GEARBOX_RATIO;
	// this->valveAngle = motorAngle - angleRaw;

	this->kalman->rtU.Encoder = this->valveAngle;
	this->kalman->step();
	this->valveAngleKalman = this->kalman->rtY.Angle;
	this->valveVelocity = this->kalman->rtY.vel;



}
