/*
 * shared_memory.h
 *
 *  Created on: May 14, 2026
 *      Author: karadeli
 */

#ifndef INC_SHARED_MEMORY_H_
#define INC_SHARED_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif
//#pragma once
#define SHARED_MEM_SIZE (384*1024)
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PACKET_SIZE sizeof(SensorData_t)
#define BUFFER_PACKET_COUNT (SHARED_MEM_SIZE / PACKET_SIZE -1)
#define BUFFER_SIZE (PACKET_SIZE * BUFFER_PACKET_COUNT)

typedef struct __attribute__((packed)) {

    float current_measured;
    float current_demand;
    float valveAngle[4];
    float valveAngleKalman[4];
    float valveVelocity[4];
    float current_subsample[8];
    float duty_subsample[8];

    float speedDemand;
    float pos_ref;

    float pos_ref_rate_limited;
    float speed_ref_rate_limited;


    float nozzle_pressure;
    float pressure_demand;
    float thrust_demand;
    float thrust_estimated;
    uint16_t nozzle_raw;

} ActuatorData_t;
typedef struct __attribute__((packed)) {
	char start[2];
	uint32_t timestamp;
	ActuatorData_t actuatorData[4];
	float manifold_pressure;
	uint16_t manifold_raw;
    float thrust_measured;
	uint16_t thrust_raw;
	uint16_t crc;
} SensorData_t;



//typedef struct __attribute

//#define BUFFER_SIZE (256*1024/sizeof(SensorData_t)-1)
typedef struct {
	volatile uint32_t head;
	volatile uint32_t tail;
	volatile uint32_t dropped;
	volatile bool record;
	volatile bool ready;


	SensorData_t sensorData[BUFFER_PACKET_COUNT]; //BUFFER_PACKET_COUNT = 1560
} SensorData_Buffer_t;




extern SensorData_Buffer_t logData;
//extern SensorData_Buffer_t logData_axiram;
void SensorData_Buffer_Init(SensorData_Buffer_t * logData);

bool SensorData_Buffer_Push(SensorData_Buffer_t *logData, const SensorData_t *entry);

bool SensorData_Buffer_Pop(SensorData_Buffer_t *logData, SensorData_t *entry);

size_t SensorData_Buffer_PopAll(SensorData_Buffer_t *logData, SensorData_t *dest, size_t max_entries);

size_t SensorData_Buffer_Count(const SensorData_Buffer_t *logData);

bool SensorData_Buffer_IsEmpty(const SensorData_Buffer_t *logData);

bool SensorData_Buffer_IsFull(const SensorData_Buffer_t *logData);
bool SensorData_Buffer_Reset_Dropped(SensorData_Buffer_t *logData);

bool SensorData_Buffer_StartRecord(SensorData_Buffer_t *logData);

bool SensorData_Buffer_StopRecord(SensorData_Buffer_t *logData);

bool SensorData_Buffer_isReady(SensorData_Buffer_t *logData);




#ifdef __cplusplus
}
#endif

//extern SensorData_t sensor_data_buffer_a[BUFFER_PACKET_COUNT/2-1];
//extern SensorData_t sensor_data_buffer_b[BUFFER_PACKET_COUNT/2-1];
//extern uint8_t ready_to_write_a;
//extern uint8_t ready_to_write_b;
//extern uint8_t printf_buffer[4096];

#endif /* INC_SHARED_MEMORY_H_ */
