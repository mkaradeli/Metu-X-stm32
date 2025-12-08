/*
 * encoder.h
 *
 *  Created on: Dec 8, 2025
 *      Author: karadeli
 */

#include <stdint.h>
#include <stdbool.h>


#ifndef ENCODER_ENCODER_H_
#define ENCODER_ENCODER_H_


#define SUBSAMPLE_COUNT 12
typedef struct {
	bool wasInMiddle;
	int16_t shifter;
	uint16_t lastReading;
	double angleRaw;
	double angleFiltered;
	double velocity;
	double calibration;

} encoder_s;


void encoderReaderInit(encoder_s* encoder, uint16_t* rawData);
void encoderReader(encoder_s* encoder, uint16_t* rawData);



#endif /* ENCODER_ENCODER_H_ */
