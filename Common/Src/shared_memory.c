/*
 * shared_memory.c
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */
#include "shared_memory.h"

__attribute__((section(".shared_memory"), used))
SensorData_t sensor_data_buffer_a[BUFFER_PACKET_COUNT/2-1];
__attribute__((section(".shared_memory"), used))
SensorData_t sensor_data_buffer_b[BUFFER_PACKET_COUNT/2-1];
__attribute__((section(".shared_memory"), used))
uint8_t ready_to_write_a;
__attribute__((section(".shared_memory"), used))
uint8_t ready_to_write_b;
__attribute__((section(".shared_memory"), used))
uint8_t printf_buffer[4096];
