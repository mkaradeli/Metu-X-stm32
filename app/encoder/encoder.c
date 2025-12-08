/*
 * encoder.c
 *
 *  Created on: Dec 8, 2025
 *      Author: karadeli
 */

#include "encoder.h"

//typedef struct {
//	bool wasInMiddle;
//	bool shifter;
//	uint16_t lastReading;
//	double angleRaw;
//	double angleFiltered;
//	double velocity;
//	uint16_t calibration;
//
//} encoder_s;


void encoderReaderInit(encoder_s* encoder, uint16_t* rawData /*12 lik bir array*/) {
	encoder->wasInMiddle = (rawData[0] < 49152) && (rawData[0] > 16384);
	encoder->shifter = 0;
	encoder->lastReading = rawData[0];
	encoder->angleRaw = rawData[0] * (360.0f / 65536);
	encoder->angleFiltered = 0;
	encoder->velocity = 0;
	double mean = 0;
	for (int i=0;i<SUBSAMPLE_COUNT;i++){
		mean += rawData[i];
	}
	mean /= 12.0f;
	encoder->calibration = mean;
};



void encoderReader(encoder_s* encoder, uint16_t* rawData) {
	int8_t middle_counter = 0;
	for (int i = 0; i< SUBSAMPLE_COUNT; i++){
		(rawData[i]<49152U) && (rawData[i]>16384U) ? middle_counter++: middle_counter--;
	}
	bool inMiddle = middle_counter > 0;
	double mean = 0;
	for (int i=0;i<SUBSAMPLE_COUNT;i++){
		mean += rawData[i];
	}
	mean /= 12.0f;

	if (inMiddle) {
		if (!encoder->wasInMiddle){
			(mean < 32768) ? encoder->shifter++ : encoder->shifter--;
		}
	}
	else {
		uint16_t temp;
		double mean_shifted = 0;
		for (int i=0; i<SUBSAMPLE_COUNT;i++){
			temp = rawData[i]+32768U;
			mean_shifted += temp;
		}
		mean_shifted /= 12.0f;
		if (encoder->wasInMiddle) {
			(mean_shifted < 32768) ? encoder->shifter++ : encoder->shifter--;
		}
	encoder->wasInMiddle = inMiddle;
	encoder->angleRaw = (mean - encoder->calibration + encoder->shifter* 32768) * 360.0f / 65536;
	}
}



