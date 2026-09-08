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

// DMA destination for HAL_UARTEx_ReceiveToIdle_DMA on USART6. Pinned to the
// MPU-marked non-cacheable SRAM2 region (main.c MPU_Config, Region 4) so the
// D-Cache can't leave FrameHandler() reading stale bytes behind a fresh DMA
// write -- same fix as common_print_buffer (ring_buffer.c).
__attribute__((section(".sram2"), used, aligned(32)))
static uint8_t lidarRxBuffer[128] = {0};

uint8_t* Lidar::getBuffer(){
	return lidarRxBuffer;
}

Lidar::Lidar(UART_HandleTypeDef* uartHandle){
	this->uart_handle = uartHandle;
}

void Lidar::FrameHandler(uint16_t size){
    if (size != LIDAR_FRAME_SIZE){
        this->status = false;
        return; // Invalid buffer size
    }
    if (lidarRxBuffer[0] != LIDAR_FRAME_HEADER || lidarRxBuffer[1] != LIDAR_FRAME_HEADER){
        this->status = false;
        return; // Invalid buffer header
    }
    this->checksum = 0;
    for (uint8_t i = 0; i < (LIDAR_FRAME_SIZE - 1); i++){
        this->checksum += lidarRxBuffer[i];
    }
    if (this->checksum != lidarRxBuffer[LIDAR_FRAME_SIZE - 1]){
        this->status = false;
        return; // Checksum mismatch
    }
    status = true;

    this->interval_us = micros() - this->flag_us;
    this->flag_us = micros();

    this->distance = lidarRxBuffer[2] | (lidarRxBuffer[3] << 8);
    this->strength = lidarRxBuffer[4] | (lidarRxBuffer[5] << 8);
    this->temperature = lidarRxBuffer[6] | (lidarRxBuffer[7] << 8);
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

void Lidar::SetUnitMm(){
	// TF02-Pro User Manual (BP-UM-30 A01) Table 4-1: "Output format" ->
	// standard 9-byte frame, distance in mm.
	uint8_t command[] = {
			0x5A, 0x05, 0x05, 0x06, 0x6A
	};
	uint8_t buffer[16] = {0};
	HAL_UART_Transmit(this->uart_handle, command, sizeof(command), 1000);
	HAL_UART_Receive(this->uart_handle, buffer, 5, 100);
}

void Lidar::SetFrameRate(uint16_t rateHz){
	if (rateHz == 0 || rateHz > 1000){
		return;
	}
	uint8_t command[6] = {
			0x5A, 0x06, 0x03, (uint8_t)(rateHz & 0xFF), (uint8_t)(rateHz >> 8), 0x00
	};
	for (uint8_t i = 0; i < (sizeof(command) - 1); i++){
		command[sizeof(command) - 1] += command[i];
	}
	uint8_t buffer[16] = {0};
	HAL_UART_Transmit(this->uart_handle, command, sizeof(command), 1000);
	HAL_UART_Receive(this->uart_handle, buffer, 6, 100);
}

void Lidar::SetOutputEnabled(bool enabled){
	// TF02-Pro User Manual Table 4-1: "Enable/Disable output". Silencing the
	// sensor's continuous stream before/after the other config commands keeps
	// their ACK from racing the live telemetry on the same, FIFO-less UART.
	uint8_t command[5] = {
			0x5A, 0x05, 0x07, (uint8_t)(enabled ? 0x01 : 0x00), (uint8_t)(enabled ? 0x67 : 0x66)
	};
	uint8_t buffer[16] = {0};
	HAL_UART_Transmit(this->uart_handle, command, sizeof(command), 1000);
	HAL_UART_Receive(this->uart_handle, buffer, 5, 100);
}

void Lidar::SaveConfig(){
	// Must be sent after SetUnitMm()/SetFrameRate(), otherwise the settings
	// revert to their previously saved values on the next power cycle.
	uint8_t command[] = {
			0x5A, 0x04, 0x11, 0x6F
	};
	uint8_t buffer[16] = {0};
	HAL_UART_Transmit(this->uart_handle, command, sizeof(command), 1000);
	HAL_UART_Receive(this->uart_handle, buffer, 5, 100);
}
