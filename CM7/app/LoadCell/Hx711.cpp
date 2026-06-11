/*
 * Hx711.cpp
 *
 *  Created on: May 29, 2026
 *      Author: karadeli
 */

#include "Hx711.hpp"

extern uint64_t micros();
extern TIM_HandleTypeDef htim6;


Hx711::Hx711(
		GPIO_TypeDef *dout_port,
		uint16_t dout_pin,
		GPIO_TypeDef *sck_port,
		uint16_t sck_pin,
		HX711_Gain gain){

	this->dout_port = dout_port;
	this->dout_pin = dout_pin;
	this->sck_port = sck_port;
	this->sck_pin = sck_pin;
	this->gain = gain;
	this->offset = 0;
	this->scale = 1.0f;
	this->recalibrate = 0;

	HAL_TIM_Base_Start(&htim6);
	this->sck_low();
};

void Hx711::sck_high() {
    HAL_GPIO_WritePin(sck_port, sck_pin, GPIO_PIN_SET);
};
void Hx711::sck_low()
{
    HAL_GPIO_WritePin(sck_port, sck_pin, GPIO_PIN_RESET);
}
void Hx711::delay_us(uint32_t us){
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	while (__HAL_TIM_GET_COUNTER(&htim6) < us) {}
}
bool Hx711::dout_read()
{
    return HAL_GPIO_ReadPin(dout_port, dout_pin) == GPIO_PIN_SET;
}
bool Hx711::isReady(){
	return !(this->dout_read());  /* DOUT LOW = data ready */
}

int32_t Hx711::raw(){
	uint32_t timeout = micros() + 100000;
	    while (!this->isReady()) {
	        if (micros()>= timeout) {
	            return 0;
	        }
	    }

	    uint32_t raw = 0;

	    __disable_irq();

	    /* Clock out 24 data bits, MSB first */
	    for (uint8_t i = 0; i < 24; i++) {
	    	this->sck_high();
	        this->delay_us(1);

	        raw <<= 1;
	        if (this->dout_read()) {
	            raw |= 1;
	        }

	        this->sck_low();
	        this->delay_us(1);
	    }

	    /* Extra pulses set gain for next conversion */
	    for (uint8_t i = 0; i < (uint8_t)gain; i++) {
	    	this->sck_high();
	        this->delay_us(1);
	        this->sck_low();
	        this->delay_us(1);
	    }

	    __enable_irq();

	    /* Sign-extend 24-bit two's complement to 32-bit */
	    if (raw & 0x800000) {
	        raw |= 0xFF000000;
	    }

	    return (int32_t)raw;
}
void Hx711::powerUp(){
	this->sck_low();
	HAL_Delay(100);
	this->raw();

}
void Hx711::tare() {
	this->offset = raw();
	this->recalibrate = 0;
}

void Hx711::setScale(float scale) {
	this->scale = scale;
}

float Hx711::read() {
	return (float) (this->raw()- this->offset)/ this->scale;
}

void Hx711::powerDown() {
	this->sck_low();
	this->sck_high();
}


