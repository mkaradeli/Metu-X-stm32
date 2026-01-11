/*
 * encoder.h
 *
 *  Created on: Dec 8, 2025
 *      Author: karadeli
 */

#ifdef __cplusplus
extern "C" {
#endif
#include "globals.h"
#include <stdint.h>
#include <stdbool.h>


#ifndef ENCODER_ENCODER_H_
#define ENCODER_ENCODER_H_


#define SUBSAMPLE_COUNT 64


typedef struct {
	bool wasInMiddle;
	int16_t shifter;
	uint16_t lastReading;
	double angleRaw;
	double angleFiltered;
	double velocity;
	double calibration;

} encoder_ss;


void encoderReaderInit(encoder_ss* encoder, uint16_t* rawData);
void encoderReader(encoder_ss* encoder, uint16_t* rawData);

#ifdef __cplusplus
}
#endif

#endif /* ENCODER_ENCODER_H_ */
