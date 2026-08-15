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
//#define SHARED_MEM_SIZE (384*1024)
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sh2_SensorValue.h"


typedef struct __attribute__((packed)) {

    float current_measured;
    float current_demand;
    float valveAngle;
    float valveAngleKalman;
    float valveVelocity;
    float duty;
//    float current_subsample[8];
//    float duty_subsample[8];

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

	sh2_RotationVector_t quaternion;
	sh2_Accelerometer_t  linearAccel;

	uint16_t lidarDistance;
	uint16_t lidarStrength;
	float kf_altitude;
	float kf_velocity;
	float kf_accelBias;
	float kf_sigmaH;
	float kf_sigmaV;
	float kf_meanNis;
	float kf_tiltCos;
	uint32_t kf_rejects;
	uint8_t kf_flags;

	uint16_t crc;
} SensorData_t;

#define PACKET_SIZE sizeof(SensorData_t)
//#define BUFFER_PACKET_COUNT (SHARED_MEM_SIZE / PACKET_SIZE -1)
//#define BUFFER_SIZE (PACKET_SIZE * BUFFER_PACKET_COUNT)

//#if sizeof(SensorData_t)>2
//
//#endif

//typedef struct __attribute
#define LOG_HALF_RECORDS   768u // 128 * 6
#define LOG_TOTAL_RECORDS  (2u * LOG_HALF_RECORDS)
#define LOG_HALF_BYTES     (LOG_HALF_RECORDS * PACKET_SIZE)

//#define BUFFER_SIZE (256*1024/sizeof(SensorData_t)-1)
typedef struct {
    /* --- producer-owned --- */
    volatile uint32_t write_idx;      /* 0 .. LOG_HALF_RECORDS-1             */
    volatile uint8_t  fill_half;      /* which array the producer is filling */

    /* --- handshake --- */
    volatile uint8_t  half_full[2];   /* set by producer, cleared by consumer*/
    volatile uint8_t  flush_half;     /* consumer-owned, enforces FIFO order */

    /* --- statistics --- */
    volatile uint32_t dropped;        /* records lost to a stalled consumer  */
    volatile uint32_t written;        /* records handed to f_write()         */

    /* --- control flags (MissionControl binds to &logData.record) --- */
	volatile bool record;
	volatile bool ready;

	SensorData_t rec[2][LOG_HALF_RECORDS];
//	SensorData_t sensorData[5]; //BUFFER_PACKET_COUNT = 1560
} SensorData_Buffer_t;




/* Kept for existing printf() call sites that reference the old name. */
//#define BUFFER_PACKET_COUNT LOG_TOTAL_RECORDS


extern SensorData_Buffer_t logData;

void SensorData_Buffer_Init(SensorData_Buffer_t *b);

/* Producer side. Call from the 1 kHz ISR.
 * Returns false and bumps ->dropped if the target half has not been
 * flushed yet (i.e. the SD write is falling behind). */
bool SensorData_Buffer_Push(SensorData_Buffer_t *b, const SensorData_t *entry);

/* Zero-copy producer variant: returns a pointer to the next free slot so the
 * ISR can build the record in place, then commit it. Returns NULL if full.
 * Pair every non-NULL Reserve() with exactly one Commit(). */
SensorData_t *SensorData_Buffer_Reserve(SensorData_Buffer_t *b);
void          SensorData_Buffer_Commit(SensorData_Buffer_t *b);

/* Consumer side. Claim returns NULL / *n = 0 when nothing is ready.
 * The returned pointer stays valid until Release() is called. */
const SensorData_t *SensorData_Buffer_ClaimHalf(SensorData_Buffer_t *b, size_t *n);
void                SensorData_Buffer_ReleaseHalf(SensorData_Buffer_t *b);

/* Flush the partially-filled half at end of recording.
 * ONLY safe once the producer is guaranteed quiet (record == false and the
 * ISR has been given a cycle to observe it). */
size_t SensorData_Buffer_ClaimTail(SensorData_Buffer_t *b, const SensorData_t **p);
void   SensorData_Buffer_ReleaseTail(SensorData_Buffer_t *b, size_t n);

size_t SensorData_Buffer_Count(const SensorData_Buffer_t *b);   /* unflushed */
bool   SensorData_Buffer_IsEmpty(const SensorData_Buffer_t *b);
bool   SensorData_Buffer_IsFull(const SensorData_Buffer_t *b);
void   SensorData_Buffer_Reset_Dropped(SensorData_Buffer_t *b);

bool SensorData_Buffer_StartRecord(SensorData_Buffer_t *b);
bool SensorData_Buffer_StopRecord(SensorData_Buffer_t *b);
bool SensorData_Buffer_isReady(SensorData_Buffer_t *b);




#ifdef __cplusplus
}
#endif

//extern SensorData_t sensor_data_buffer_a[BUFFER_PACKET_COUNT/2-1];
//extern SensorData_t sensor_data_buffer_b[BUFFER_PACKET_COUNT/2-1];
//extern uint8_t ready_to_write_a;
//extern uint8_t ready_to_write_b;
//extern uint8_t printf_buffer[4096];

#endif /* INC_SHARED_MEMORY_H_ */
