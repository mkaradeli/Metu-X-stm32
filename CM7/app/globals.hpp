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

#include "app_main.hpp"
#include "Actuator.hpp"
#define ADC_16B_MAX 65536U



//extern Motor motors[4];
extern Actuator actuator[4];
//PressureSensor psSensors[5];




#endif /* GLOBALS_HPP_ */
