/*
 * Hx711.hpp
 *
 *  Created on: May 29, 2026
 *      Author: karadeli
 */

#ifndef LOADCELL_HX711_HPP_
#define LOADCELL_HX711_HPP_

#include "stm32h7xx_hal.h"

enum HX711_Gain {
	HX711_GAIN_128 = 1,
	HX711_GAIN_32 = 2,
	HX711_GAIN_64 = 3,
};

class Hx711 {
public:
	Hx711(
			GPIO_TypeDef *dout_port,
			uint16_t dout_pin,
			GPIO_TypeDef *sck_port,
			uint16_t sck_pin,
			HX711_Gain gain);
//	void init(HX711_Gain gain);
	bool isReady();
	int32_t raw();
	void tare();
	void setScale(float scale);
	float read();
	void powerDown();
	void powerUp();
private:
	GPIO_TypeDef *dout_port;
	uint16_t      dout_pin;
	GPIO_TypeDef *sck_port;
	uint16_t      sck_pin;
	HX711_Gain    gain;
	int32_t       offset;
	float         scale;
	uint8_t       recalibrate;
	void sck_high();
	void sck_low();
	void delay_us(uint32_t us);
	bool dout_read();

};


#endif /* LOADCELL_HX711_HPP_ */
