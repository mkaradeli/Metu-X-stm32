/*
 * Button.hpp
 *
 *  Created on: Jul 3, 2026
 *      Author: karadeli
 */

#ifndef BUTTON_BUTTON_HPP_
#define BUTTON_BUTTON_HPP_

#pragma once
#include "stm32h7xx_hal.h"


class DebouncedButton {
public:
    DebouncedButton(bool activeLow = false)
        : activeLow_(activeLow) {}

    // Call every 1 ms (SysTick callback, 1 kHz timer, or your control loop).
    void update() {
        bool raw = (HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET);
        if (activeLow_) raw = !raw;

        history_ = static_cast<uint8_t>((history_ << 1) | (raw ? 1u : 0u));

        if (history_ == 0xFFu && !stable_) {        // 8 consecutive "pressed" samples
            stable_ = true;
            pressedEvent_ = true;
        } else if (history_ == 0x00u && stable_) {  // 8 consecutive "released" samples
            stable_ = false;
            releasedEvent_ = true;
        }
    }

    bool isPressed() const { return stable_; }

    // One-shot events, cleared on read.
    bool pressed()  { return takeFlag(pressedEvent_); }
    bool released() { return takeFlag(releasedEvent_); }

private:
    bool takeFlag(volatile bool& flag) {
        // Guard against the update() ISR setting the flag between read and clear.
//        uint32_t primask = __get_PRIMASK();
//        __disable_irq();
        bool e = flag;
        flag = false;
//        __set_PRIMASK(primask);
        return e;
    }

    GPIO_TypeDef* port_ = BUTTON_USER_GPIO_PORT;
    uint16_t pin_ = BUTTON_USER_PIN;
    bool activeLow_;
    uint8_t history_ = 0;
    volatile bool stable_ = false;
    volatile bool pressedEvent_ = false;
    volatile bool releasedEvent_ = false;
};

#endif /* BUTTON_BUTTON_HPP_ */
