/*
 * globals.hpp
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */

#define ENABLE_MOTORS true

#ifndef GLOBALS_HPP_
#define GLOBALS_HPP_

#include <LowPassFilter/LowPassFilter.hpp>
#include <cmath>
#include <cstdio>

#include "Profiler.hpp"
#include "currentController.h"
#include "Motor.hpp"
#include "LoadCell.hpp"
#include "Lidar.hpp"

#include "AltitudeEstimator.hpp"

#include "platformController.h"
#include "app_main.hpp"
#include "Actuator.hpp"
#include "HWIL.h"
#define ADC_16B_MAX 65536U

extern Actuator actuator[4];
extern LoadCell loadCell;

extern const uint16_t logFormatId;
//extern const char logHeader[];
//extern const uint8_t logHeaderSize;
extern const uint16_t sensorDataLength;
extern Lidar lidar;

extern const float ValveFitPressureRatios[4][11];
extern const int encoderSigns[4];
extern PlatformController platform_controller;
extern const float nozzle_gain[4];
extern AltitudeEstimator::Params p;
extern AltitudeEstimator g_altEst;
extern HWIL hwil;
/*
1 x- left
2 y+ front
3 y- back
4 x+ right
*/

#endif /* GLOBALS_HPP_ */
