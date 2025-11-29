/*
 * Lidar.hpp
 *
 *  Created on: Aug 7, 2025
 *      Author: alica
 */

#ifndef LIDAR_LIDAR_HPP_
#define LIDAR_LIDAR_HPP_

#define LIDAR_FRAME_HEADER 0x59
#define LIDAR_FRAME_SIZE 9

#include <stdint.h>
#include "usart.h"

class Lidar {
	public:
		Lidar(UART_HandleTypeDef* uartHandle);
		void FrameHandler(uint16_t size);
		void Reset();
		uint8_t* getBuffer(){
			return this->buffer;
		}
		uint16_t getDistance(){
			return this->distance;
		}

		uint16_t getStrength(){
			return this->strength;
		}

	private:
		UART_HandleTypeDef* uart_handle;
		uint8_t buffer[128] = {0};
		uint16_t distance = 0;
		uint16_t strength = 0;
		uint16_t temperature = 0;
		uint8_t checksum = 0;
		uint32_t interval_us = 0;
		uint32_t flag_us = 0;
		bool status = false;
};


#endif /* LIDAR_LIDAR_HPP_ */
