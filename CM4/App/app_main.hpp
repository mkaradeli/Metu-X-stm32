/*
 * app_main.hpp
 *
 *  Created on: Jun 13, 2026
 *      Author: karadeli
 */

#ifndef APP_MAIN_HPP_
#define APP_MAIN_HPP_
//#pragma once

#include "stm32h7xx_hal.h"
//#include "fatfs.h"



#define CLR_SCREEN      "\033[2J\033[H"



#ifdef __cplusplus
extern "C" {
#endif

	void app_init();
	void app_loop();

	uint64_t cpuTicks();
	uint64_t micros();
//	extern TIM_HandleTypeDef htim5;
//    extern TIM_HandleTypeDef htim7;





#ifdef __cplusplus
}
#endif


#endif /* APP_MAIN_HPP_ */
