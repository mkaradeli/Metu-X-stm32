/*
 * hx711.h
 *
 *  Created on: May 10, 2026
 *      Author: karadeli
 */

/*
 * hx711.h — STM32 HAL driver for HX711 24-bit load cell ADC
 *
 * Protocol: proprietary bit-bang serial (not SPI/I2C)
 *   - DOUT goes LOW when data is ready
 *   - Clock out 24 bits MSB-first on SCK rising edge
 *   - Extra pulses (25/26/27 total) select gain for NEXT conversion
 *   - SCK HIGH > 60µs = power-down
 */

#ifndef HX711_H_
#define HX711_H_

#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    HX711_GAIN_128 = 1,  /* Channel A, gain 128 — 25 total pulses */
    HX711_GAIN_32  = 2,  /* Channel B, gain 32  — 26 total pulses */
    HX711_GAIN_64  = 3,  /* Channel A, gain 64  — 27 total pulses */
} HX711_Gain;

typedef struct {
    GPIO_TypeDef *dout_port;
    uint16_t      dout_pin;
    GPIO_TypeDef *sck_port;
    uint16_t      sck_pin;
    HX711_Gain    gain;
    int32_t       offset;
    float         scale;
} HX711_Handle;

void    HX711_Init(HX711_Handle *hx, HX711_Gain gain);
bool    HX711_IsReady(const HX711_Handle *hx);
int32_t HX711_ReadRaw(HX711_Handle *hx);
int32_t HX711_ReadRawAvg(HX711_Handle *hx, uint8_t n);
void    HX711_Tare(HX711_Handle *hx, uint8_t n);
void    HX711_SetScale(HX711_Handle *hx, float scale);
float   HX711_ReadUnits(HX711_Handle *hx, uint8_t n);
void    HX711_PowerDown(HX711_Handle *hx);
void    HX711_PowerUp(HX711_Handle *hx);


#endif /* HX711_H_ */
