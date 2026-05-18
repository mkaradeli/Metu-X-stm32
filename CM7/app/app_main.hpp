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

#ifdef __cplusplus
extern "C" {
#endif

	void app_init();
	void app_loop();




#ifdef __cplusplus
}
#endif

#endif /* APP_MAIN_HPP_ */
