/*
 * encoder.hpp
 *
 *  Created on: Jan 11, 2026
 *      Author: karadeli
 */


#ifndef ENCODER_ENCODER_HPP_
#define ENCODER_ENCODER_HPP_

#define ADC_16B_MAX 65536U
#define HALF_ADC_16B_MAX 32768U
#define SUBSAMPLE_COUNT 64
#define GEARBOX_RATIO 13.7f

#include "globals.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "actuatorKalman.h"



class HallEffect{

public:
	HallEffect(uint16_t* adc_buffer, Kalman* actuatorKalman);
//	HallEffect(const HallEffect&) = delete;
//	HallEffect& operator=(const HallEffect&) = delete;
//
//	HallEffect(HallEffect&&) = delete;
//	HallEffect& operator=(HallEffect&&) = delete;


//	uint16_t lastReading;
//	double angleRaw;
//	double angle_Filtered;
//	double velocity;
	double calibration;
	double motorAngle;
	double valveAngle;
	double valveAngleKalman;
	double valveVelocity;
	uint32_t time_update;
	uint32_t time_subBuffer;
	uint32_t time_current;
	uint16_t unstableCount;

	void calibrate();
	void update();
	void update_subBuffer();


private:
	uint16_t* adc_buffer;
	bool wasInMiddle;
	bool wasInMiddle_subBuffer;
	int32_t shifter;
	int32_t shifter_subBuffer;
	uint32_t mean_subBuffer;
	Kalman* kalman;
};

#endif /* ENCODER_ENCODER_HPP_ */
