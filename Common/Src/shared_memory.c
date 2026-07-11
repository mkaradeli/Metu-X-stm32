
/*
 * shared_memory.c
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */
#include "shared_memory.h"
#include "cmsis_compiler.h"   /* __DMB() */
#include "string.h"


__attribute__((section(".log_data")))
SensorData_Buffer_t logData;

//__attribute__((section(".log_data"), aligned(4)))
//SensorData_Buffer_t logData_axiram;


static inline uint32_t SensorData_next(uint32_t i) {
	uint32_t n = i + 1u;
	return (n >= BUFFER_PACKET_COUNT) ? 0u : n;
}

void SensorData_Buffer_Init(SensorData_Buffer_t * logData) {
	logData->head = 0;
	logData->tail = 0;
	logData->dropped = 0;
	logData->record=0;
	logData->written=0;

	__DMB();
};

bool SensorData_Buffer_Push(SensorData_Buffer_t *logData, const SensorData_t *entry) { // only used in CM7
	const uint32_t head = logData->head;
	const uint32_t tail = logData->tail;          /* snapshot consumer ptr */
	const uint32_t next = SensorData_next(head);

	if (next == tail) {                            /* buffer full           */
		logData->dropped++;
		return false;
	}

	logData->sensorData[head] = *entry;            /* write payload first   */

	__DMB();                                       /* payload visible ...   */
	logData->head = next;                          /* ... before publish    */
	return true;
}

bool SensorData_Buffer_Pop(SensorData_Buffer_t *logData, SensorData_t *entry) { // only used in CM4
	const uint32_t tail = logData->tail;
	const uint32_t head = logData->head;           /* snapshot producer ptr */

	if (head == tail) {                            /* empty                 */
		return false;
	}

	__DMB();                                       /* order head-read       */
	                                               /* before payload-read   */
	*entry = logData->sensorData[tail];

	__DMB();                                       /* payload read done ... */
	logData->tail = SensorData_next(tail);         /* ... before slot freed */
	return true;
}

size_t SensorData_Buffer_PopAll(SensorData_Buffer_t *logData, SensorData_t *dest, size_t max_entries) {
	const uint32_t tail = logData->tail;
	const uint32_t head = logData->head;

	size_t n = (head >= tail) ? (size_t)(head - tail)
	                          : (size_t)(head + BUFFER_PACKET_COUNT - tail);

	if (n == 0u || max_entries == 0u) {
		return 0u;
	}
	if (n > max_entries) {
		n = max_entries;
	}

	__DMB();                                       /* see published records */

	const uint32_t to_end = BUFFER_PACKET_COUNT - tail;   /* before wrap    */

	if (n <= (size_t)to_end) {
		memcpy(dest,
		       &logData->sensorData[tail],
		       n * sizeof(SensorData_t));
	} else {
		memcpy(dest,
		       &logData->sensorData[tail],
		       (size_t)to_end * sizeof(SensorData_t));
		memcpy(dest + to_end,
		       &logData->sensorData[0],
		       (n - to_end) * sizeof(SensorData_t));
	}

	__DMB();                                       /* reads done ...        */
	uint32_t new_tail = tail + (uint32_t)n;        /* ... before freeing    */
	if (new_tail >= BUFFER_PACKET_COUNT) {
		new_tail -= BUFFER_PACKET_COUNT;
	}
	logData->written += n;
	logData->tail = new_tail;
	return n;
}

size_t SensorData_Buffer_Count(const SensorData_Buffer_t *logData) {
	const uint32_t h = logData->head;
	const uint32_t t = logData->tail;
	return (h >= t) ? (h - t) : (h + BUFFER_PACKET_COUNT - t);

};

bool SensorData_Buffer_IsEmpty(const SensorData_Buffer_t *logData) {
	return logData->head == logData->tail;
};

bool SensorData_Buffer_IsFull(const SensorData_Buffer_t *logData) {
	return SensorData_next(logData->head) == logData->tail;
};

bool SensorData_Buffer_Reset_Dropped(SensorData_Buffer_t *logData){
	logData->dropped = 0;
	return logData->dropped;
}
bool SensorData_Buffer_StartRecord(SensorData_Buffer_t *logData) {
	if (logData->ready)
		return logData->record = true;
	else
		return false;
}

bool SensorData_Buffer_StopRecord(SensorData_Buffer_t *logData) {
	if (logData->record) {
		logData->record = false;
		return true;
	}
	else
		return false;
}

bool SensorData_Buffer_isReady(SensorData_Buffer_t *logData) {
	return logData->ready;
}

