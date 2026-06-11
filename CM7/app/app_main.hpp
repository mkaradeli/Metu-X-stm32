/*
 * app_main.hpp
 *
 *  Created on: May 14, 2026
 *      Author: karadeli
 */

#ifndef APP_MAIN_HPP_
#define APP_MAIN_HPP_
#include "stm32h7xx_hal.h"



#define CLR_SCREEN      "\033[2J\033[H"

#define MICROS DWT->CYCCNT

#ifdef __cplusplus
extern "C" {
#endif

	void app_init();
	void app_loop();

	void current_adc_complete();
	void encoder_adc_complete();
	void pressure_adc_complete();

	void tim3_trigger();
	void tim4_trigger();
	void tim2_trigger();
	void tim5_trigger();
	uint64_t micros();

    extern ADC_HandleTypeDef hadc1;
    extern ADC_HandleTypeDef hadc2;
    extern ADC_HandleTypeDef hadc3;
    extern TIM_HandleTypeDef htim1;
    extern TIM_HandleTypeDef htim2;
    extern TIM_HandleTypeDef htim3;
    extern TIM_HandleTypeDef htim4;
    extern TIM_HandleTypeDef htim6;
    extern TIM_HandleTypeDef htim5;

#ifdef __cplusplus
}
#endif



#endif /* APP_MAIN_HPP_ */
