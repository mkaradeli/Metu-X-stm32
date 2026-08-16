/*
 * globals.hpp
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */

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
#define ADC_16B_MAX 65536U

extern Actuator actuator[4];
extern LoadCell loadCell;

extern const uint16_t logFormatId;
extern const char logHeader[];
extern const uint8_t logHeaderSize;
extern const uint16_t sensorDataLength;
extern Lidar lidar;

extern const float ValveFitPressureRatios[4][12];
extern const int encoderSigns[4];
extern platformController platform_controller;


#endif /* GLOBALS_HPP_ */
