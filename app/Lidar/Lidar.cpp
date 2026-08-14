/*
 * Lidar.cpp
 *
 *  Created on: Aug 7, 2025
 *      Author: alica
 */
#include "Lidar.hpp"
#include "tim.h"
#include "usart.h"

extern "C" {
extern uint64_t micros();
}
Lidar::Lidar(UART_HandleTypeDef* uartHandle){
	this->uart_handle = uartHandle;
}

void Lidar::FrameHandler(uint16_t size){
    if (size != LIDAR_FRAME_SIZE){
        this->status = false;
        return; // Invalid this->buffer size
    }
    if (this->buffer[0] != LIDAR_FRAME_HEADER && this->buffer[1] != LIDAR_FRAME_HEADER){
        this->status = false;
        return; // Invalid this->buffer header
    }
    for (uint8_t i = 0; i < (LIDAR_FRAME_SIZE - 1); i++){
        this->checksum += this->buffer[i];
    }
    if (this->checksum != this->buffer[LIDAR_FRAME_SIZE - 1]){
        this->status = false;
        return; // Checksum mismatch
    }
    this->checksum = 0;

    this->interval_us = micros() - this->flag_us;
    this->flag_us = micros();

    this->distance = this->buffer[2] | (this->buffer[3] << 8);
    this->strength = this->buffer[4] | (this->buffer[5] << 8);
    this->temperature = this->buffer[6] | (this->buffer[7] << 8);

}

void Lidar::Reset(){
	uint8_t command[] = {
			0x5A, 0x04, 0x02, 0x60
	};
	uint8_t buffer[15] = {0};
	HAL_UART_Transmit(this->uart_handle, command, sizeof(command), 1000);
	HAL_UART_Receive(this->uart_handle, buffer, 5, 100);
}
