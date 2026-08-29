/*
 * shared_memory.c
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */
#include <shared_memory.h>
#include "cmsis_compiler.h"   /* __DMB() */
#include <string.h>

/* 32-byte aligned so the halves can later be handed to a DMA-capable SDMMC
 * or SPI driver without straddling cache lines. Lives in .bss -> AXI SRAM. */
//__attribute__((aligned(32)))
__attribute__((section(".log_data"), used))
SensorData_Buffer_t logData;
//static_assert(sizeof(SensorData_t)%4==0 ,"degerler sadece +1 ve -1 olabilir.");


void SensorData_Buffer_Init(SensorData_Buffer_t *b)
{
    b->write_idx    = 0u;
    b->fill_half    = 0u;
    b->half_full[0] = 0u;
    b->half_full[1] = 0u;
    b->flush_half   = 0u;
    b->dropped      = 0u;
    b->written      = 0u;
    b->record       = false;
    b->ready        = false;      /* the old Init() forgot this one */

    __DMB();
}


/* ------------------------------------------------------------------ */
/* Producer (ISR context)                                              */
/* ------------------------------------------------------------------ */

bool SensorData_Buffer_Push(SensorData_Buffer_t *b, const SensorData_t *entry)
{
    SensorData_t *slot = SensorData_Buffer_Reserve(b);
    if (slot == NULL) {
        return false;                      /* Reserve() already counted it */
    }
    *slot = *entry;
    SensorData_Buffer_Commit(b);
    return true;
}


SensorData_t *SensorData_Buffer_Reserve(SensorData_Buffer_t *b)
{
    const uint32_t h = b->fill_half;

    if (b->half_full[h]) {                 /* consumer hasn't caught up */
        b->dropped++;
        return NULL;
    }
    return &b->rec[h][b->write_idx];
}


void SensorData_Buffer_Commit(SensorData_Buffer_t *b)
{
    const uint32_t next = b->write_idx + 1u;

    if (next == LOG_HALF_RECORDS) {        /* this record closed the half */
        const uint32_t h = b->fill_half;

        __DMB();                           /* payload visible ...          */
        b->half_full[h] = 1u;              /* ... before the handoff flag  */

        b->fill_half = h ^ 1u;
        b->write_idx = 0u;
    } else {
        b->write_idx = next;
    }
}


/* ------------------------------------------------------------------ */
/* Consumer (main loop)                                                */
/* ------------------------------------------------------------------ */

const SensorData_t *SensorData_Buffer_ClaimHalf(SensorData_Buffer_t *b, size_t *n)
{
    const uint32_t h = b->flush_half;

    if (!b->half_full[h]) {
        *n = 0u;
        return NULL;
    }

    __DMB();                               /* flag read before payload read */
    *n = LOG_HALF_RECORDS;
    return b->rec[h];
}


void SensorData_Buffer_ReleaseHalf(SensorData_Buffer_t *b)
{
    const uint32_t h = b->flush_half;

    b->written += LOG_HALF_RECORDS;

    __DMB();                               /* reads done ...               */
    b->half_full[h] = 0u;                  /* ... before slot is freed     */
    b->flush_half   = h ^ 1u;              /* strict alternation = FIFO    */
}


/* ------------------------------------------------------------------ */
/* End-of-recording tail flush                                         */
/* ------------------------------------------------------------------ */
/*
 * Returns the records sitting in the half that is currently being filled.
 * There is no interlock here: the caller must have stopped the producer
 * first (record = false, then let at least one ISR period elapse).
 */
size_t SensorData_Buffer_ClaimTail(SensorData_Buffer_t *b, const SensorData_t **p)
{
    const uint32_t i = b->write_idx;

    if (i == 0u) {                         /* half is empty */
        *p = NULL;
        return 0u;
    }

    __DMB();
    *p = b->rec[b->fill_half];
    return (size_t)i;
}


void SensorData_Buffer_ReleaseTail(SensorData_Buffer_t *b, size_t n)
{
    b->written += (uint32_t)n;
    __DMB();
    b->write_idx = 0u;
}


/* ------------------------------------------------------------------ */
/* Status                                                              */
/* ------------------------------------------------------------------ */

size_t SensorData_Buffer_Count(const SensorData_Buffer_t *b)
{
    size_t n = (size_t)b->write_idx;       /* partially filled half */

    if (b->half_full[0]) n += LOG_HALF_RECORDS;
    if (b->half_full[1]) n += LOG_HALF_RECORDS;
    return n;
}


bool SensorData_Buffer_IsEmpty(const SensorData_Buffer_t *b)
{
    return SensorData_Buffer_Count(b) == 0u;
}


bool SensorData_Buffer_IsFull(const SensorData_Buffer_t *b)
{
    return b->half_full[b->fill_half] != 0u;
}


void SensorData_Buffer_Reset_Dropped(SensorData_Buffer_t *b)
{
    b->dropped = 0u;
}


bool SensorData_Buffer_StartRecord(SensorData_Buffer_t *b)
{
    if (!b->ready) {
        return false;
    }
    b->record = true;
    return true;
}


bool SensorData_Buffer_StopRecord(SensorData_Buffer_t *b)
{
    if (!b->record) {
        return false;
    }
    b->record = false;
    return true;
}


bool SensorData_Buffer_isReady(SensorData_Buffer_t *b)
{
    return b->ready;
}
