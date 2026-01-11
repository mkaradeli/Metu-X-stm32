/*
 * encoder.hpp
 *
 *  Created on: Jan 11, 2026
 *      Author: karadeli
 */


#ifndef ENCODER_ENCODER_HPP_
#define ENCODER_ENCODER_HPP_

#define ADC_16B_MAX 65536U
#define HALF_ADC_16B_MAX ADC_16B_MAX/2
#define SUBSAMPLE_COUNT 64

#include "globals.h"
#include <stdint.h>
#include <stdbool.h>



class HallEffect{
public:
	HallEffect(uint16_t* adc_buffer);
	void calibrate();
	void update();
private:
	bool wasInMiddle;
	int16_t shifter;
	uint16_t lastReading;
	double angleRaw;
	double angle Filtered;
	double velocity;
	double calibration;
	uint16_t* adc_buffer;


}

#endif /* ENCODER_ENCODER_HPP_ */
