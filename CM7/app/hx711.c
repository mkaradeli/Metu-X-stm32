/*
 * hx711.cpp
 *
 *  Created on: May 10, 2026
 *      Author: karadeli
 */


#include "hx711.h"

extern TIM_HandleTypeDef htim6;

/* ---------- Microsecond delay (TIM6-based) ---------- */

static void delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (__HAL_TIM_GET_COUNTER(&htim6) < us) {}
}

/* ---------- GPIO helpers ---------- */

static inline void sck_high(HX711_Handle *hx)
{
    HAL_GPIO_WritePin(hx->sck_port, hx->sck_pin, GPIO_PIN_SET);
}

static inline void sck_low(HX711_Handle *hx)
{
    HAL_GPIO_WritePin(hx->sck_port, hx->sck_pin, GPIO_PIN_RESET);
}

static inline bool dout_read(const HX711_Handle *hx)
{
    return HAL_GPIO_ReadPin(hx->dout_port, hx->dout_pin) == GPIO_PIN_SET;
}

/* ---------- Public API ---------- */

void HX711_Init(HX711_Handle *hx, HX711_Gain gain)
{
    hx->gain   = gain;
    hx->offset = 0;
    hx->scale  = 1.0f;

    HAL_TIM_Base_Start(&htim6);

    sck_low(hx);
    HAL_Delay(100);

    HX711_ReadRaw(hx);  /* Dummy read to apply gain */
}

bool HX711_IsReady(const HX711_Handle *hx)
{
    return !dout_read(hx);  /* DOUT LOW = data ready */
}

int32_t HX711_ReadRaw(HX711_Handle *hx)
{
    uint32_t timeout = HAL_GetTick() + 500;
    while (!HX711_IsReady(hx)) {
        if (HAL_GetTick() > timeout) {
            return 0;
        }
    }

    uint32_t raw = 0;

    __disable_irq();

    /* Clock out 24 data bits, MSB first */
    for (uint8_t i = 0; i < 24; i++) {
        sck_high(hx);
        delay_us(1);

        raw <<= 1;
        if (dout_read(hx)) {
            raw |= 1;
        }

        sck_low(hx);
        delay_us(1);
    }

    /* Extra pulses set gain for next conversion */
    for (uint8_t i = 0; i < (uint8_t)hx->gain; i++) {
        sck_high(hx);
        delay_us(1);
        sck_low(hx);
        delay_us(1);
    }

    __enable_irq();

    /* Sign-extend 24-bit two's complement to 32-bit */
    if (raw & 0x800000) {
        raw |= 0xFF000000;
    }

    return (int32_t)raw;
}

int32_t HX711_ReadRawAvg(HX711_Handle *hx, uint8_t n)
{
    int64_t sum = 0;
    for (uint8_t i = 0; i < n; i++) {
        sum += HX711_ReadRaw(hx);
    }
    return (int32_t)(sum / n);
}

void HX711_Tare(HX711_Handle *hx, uint8_t n)
{
    hx->offset = HX711_ReadRawAvg(hx, n);
}

void HX711_SetScale(HX711_Handle *hx, float scale)
{
    hx->scale = scale;
}

float HX711_ReadUnits(HX711_Handle *hx, uint8_t n)
{
    int32_t raw = HX711_ReadRawAvg(hx, n);
    return (float)(raw - hx->offset) / hx->scale;
}

void HX711_PowerDown(HX711_Handle *hx)
{
    sck_low(hx);
    sck_high(hx);
    delay_us(65);
}

void HX711_PowerUp(HX711_Handle *hx)
{
    sck_low(hx);
    HAL_Delay(100);
    HX711_ReadRaw(hx);
}

