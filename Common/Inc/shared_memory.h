/*
 * shared_memory.h
 *
 *  Created on: May 14, 2026
 *      Author: karadeli
 */

#ifndef INC_SHARED_MEMORY_H_
#define INC_SHARED_MEMORY_H_
//#pragma once
#define SHARED_MEM_SIZE 10*1024
#include <stdint.h>
#include <stdbool.h>

typedef struct __attribute__((aligned(4))) {
    uint32_t timestamp;
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

    float manifold_pressure;
    float nozzle_pressure;

    float pressure_demand;

    float thrust_demand;
    float thrust_estimated;
    float thrust_measured;
} SensorData_t;


#define PACKET_SIZE sizeof(SensorData_t)
#define BUFFER_PACKET_COUNT (SHARED_MEM_SIZE / PACKET_SIZE )
#define BUFFER_SIZE (PACKET_SIZE * BUFFER_PACKET_COUNT)



extern SensorData_t sensor_data_buffer_a[BUFFER_PACKET_COUNT/2-1];
extern SensorData_t sensor_data_buffer_b[BUFFER_PACKET_COUNT/2-1];
extern uint8_t ready_to_write_a;
extern uint8_t ready_to_write_b;
extern uint8_t printf_buffer[4096];

#endif /* INC_SHARED_MEMORY_H_ */
