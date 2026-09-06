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
    status = true;
    this->checksum = 0;

    this->interval_us = micros() - this->flag_us;
    this->flag_us = micros();

    this->distance = this->buffer[2] | (this->buffer[3] << 8);
    this->strength = this->buffer[4] | (this->buffer[5] << 8);
    this->temperature = this->buffer[6] | (this->buffer[7] << 8);
    this->newReading = true;

}

void Lidar::Reset(){
	uint8_t command[] = {
			0x5A, 0x04, 0x02, 0x60
	};
	// Sized to a multiple of 4 (only the first 5 bytes are used below) so the
	// compiler's zero-init doesn't emit an unaligned tail word-store.
	uint8_t buffer[16] = {0};
	HAL_UART_Transmit(this->uart_handle, command, sizeof(command), 1000);
	HAL_UART_Receive(this->uart_handle, buffer, 5, 100);
}

// Pings the sensor with the "obtain firmware version" command (5A 04 01 5F).
// Unlike Reset(), this doesn't reboot the sensor, so it's safe to use as a
// pure liveness/wiring check. Must be called before the continuous
// HAL_UARTEx_ReceiveToIdle_DMA capture is started on this UART, since it
// polls the RX line with blocking reads.
bool Lidar::HealthCheck(uint8_t *fwVersion){
	const uint8_t command[] = {0x5A, 0x04, 0x01, 0x5F};
	const uint8_t responseHeader[3] = {0x5A, 0x07, 0x01};
	uint8_t payload[4] = {0}; // V1, V2, V3, checksum

	if (HAL_UART_Transmit(this->uart_handle, (uint8_t*)command, sizeof(command), 100) != HAL_OK){
		return false;
	}

	// Continuous distance frames (0x59 0x59 ...) may be interleaved with the
	// reply on this line, so scan byte-by-byte for the response header
	// instead of assuming the reply arrives first.
	uint8_t matched = 0;
	uint8_t rxByte = 0;
	uint32_t start = HAL_GetTick();
	while ((HAL_GetTick() - start) < 500){
		if (HAL_UART_Receive(this->uart_handle, &rxByte, 1, 20) != HAL_OK){
			continue;
		}
		if (rxByte == responseHeader[matched]){
			matched++;
			if (matched == 3){
				break;
			}
		} else {
			matched = (rxByte == responseHeader[0]) ? 1 : 0;
		}
	}
	if (matched != 3){
		return false; // No response -> check wiring, power and baud rate
	}

	if (HAL_UART_Receive(this->uart_handle, payload, sizeof(payload), 100) != HAL_OK){
		return false; // Header matched but payload never completed
	}

	uint8_t checksum = responseHeader[0] + responseHeader[1] + responseHeader[2]
			+ payload[0] + payload[1] + payload[2];
	if (checksum != payload[3]){
		return false; // Corrupted reply -> likely a baud-rate/noise issue
	}

	if (fwVersion != nullptr){
		fwVersion[0] = payload[0]; // V1
		fwVersion[1] = payload[1]; // V2
		fwVersion[2] = payload[2]; // V3 - displayed as V3.V2.V1
	}
	return true;
}
