/*
 * sd_task.cpp
 *
 *  Created on: Aug 2, 2026
 *      Author: karadeli
 *
 * Lifecycle
 * ---------
 *   NotMounted --mount ok--> NoFile --open ok--> Logging
 *        ^                                          |
 *        +-------------- any write/sync fault -------+
 *
 * On fault the log index is retained, so the next pass reopens the *same*
 * file with FA_OPEN_APPEND rather than starting a new one. The reopen path
 * trims any partial trailing record, aligning to LOG_HEADER_BYTES rather than
 * to offset 0 - aligning to 0 shifts the record grid by 512 % 252 = 8 bytes
 * and makes everything after the splice unparseable.
 */

#include "sd_task.hpp"
#include "fatfs.h"
#include "ff_gen_drv.h"
#include "shared_memory.h"
#include "globals.hpp"
#include <string.h>
#include <stdio.h>

extern Profiler sd_card_profiler;
extern Disk_drvTypeDef disk;

/* ------------------------------------------------------------------ */
/* State                                                               */
/* ------------------------------------------------------------------ */

static FATFS   FatFs;
static FIL     Fil;
static SdState state = SdState::NotMounted;

static uint16_t log_index      = 0;
static bool     have_log_index = false;   /* reopen this index after a fault */

/* Tail-flush handshake. The producer is stopped one prep tick before the
 * partial half is read, so ClaimTail's "producer must be quiet" contract is
 * satisfied by ~500 ms of margin. */
static bool prev_record = false;
static bool tail_armed  = false;

SdState sd_card_state() { return state; }

/* ------------------------------------------------------------------ */
/* Fault handling                                                      */
/* ------------------------------------------------------------------ */

static void sd_fault(const char *what, FRESULT res)
{
    printf("SD fault: %s (res=%d)\n\r", what, (int)res);
    BSP_LED_On(LED_RED);

    logData.ready = false;
    tail_armed    = false;

    f_close(&Fil);
    f_mount(NULL, "", 1);
    disk.is_initialized[0] = 0;      /* force disk_initialize() on remount */

    state = SdState::NotMounted;
}

/* ------------------------------------------------------------------ */
/* Mounting                                                            */
/* ------------------------------------------------------------------ */

static void sd_try_mount()
{
    static uint32_t attempts = 0;

    disk.is_initialized[0] = 0;
    FRESULT res = f_mount(&FatFs, "", 1);

    if (res != FR_OK) {
        if ((attempts++ % 20u) == 0u) {          /* ~10 s between complaints */
            printf("SD mount failed (res=%d), retrying\n\r", (int)res);
        }
        BSP_LED_On(LED_RED);
        return;
    }

    attempts = 0;
    printf("SD card mounted\n\r");
    state = SdState::NoFile;
}

/* ------------------------------------------------------------------ */
/* File creation / reopen                                              */
/* ------------------------------------------------------------------ */

/* Lowest index in [1000, 10000) with no existing file, by bisection.
 * Valid because log files are created in strictly increasing order, so
 * existence is monotonic. Returns false on a real media error. */
static bool find_free_log_index(uint16_t *out)
{
    char name[32];
    uint16_t lo = 1000, hi = 10000;

    while (lo < hi) {
        uint16_t mid = lo + ((hi - lo) >> 1);
        snprintf(name, sizeof(name), "log%04u.bin", mid);

        FRESULT res = f_stat(name, NULL);
        if (res == FR_OK)            lo = mid + 1;
        else if (res == FR_NO_FILE)  hi = mid;
        else                         return false;   /* media error */
    }

    *out = lo;
    return true;
}

static FRESULT write_log_header()
{
    uint8_t hdr[LOG_HEADER_BYTES] = { 0 };
    UINT    wc = 0;
    size_t  o  = 0;

    memcpy(hdr + o, &logFormatId,      sizeof(uint16_t)); o += sizeof(uint16_t);
    memcpy(hdr + o, &logHeaderSize,    sizeof(uint8_t));  o += sizeof(uint8_t);
    memcpy(hdr + o, &logHeader,        logHeaderSize);    o += logHeaderSize;
    memcpy(hdr + o, &sensorDataLength, sizeof(uint16_t));

    FRESULT res = f_write(&Fil, hdr, sizeof(hdr), &wc);
    if (res == FR_OK && wc != sizeof(hdr)) {
        res = FR_INT_ERR;            /* short header => misaligned records */
    }
    return res;
}

static void sd_create_file()
{
    char name[32];

    if (!find_free_log_index(&log_index)) {
        sd_fault("scanning for free log index", FR_DISK_ERR);
        return;
    }

    snprintf(name, sizeof(name), "log%04u.bin", log_index);

    FRESULT res = f_open(&Fil, name, FA_CREATE_NEW | FA_WRITE);
    if (res != FR_OK) {
        sd_fault("creating log file", res);
        return;
    }

    /* f_expand(&Fil, 16u*1024u*1024u, 1); // contiguous preallocation:
     * makes f_sync cheap, but sets the file size to the full extent up front,
     * so the parser must stop at the last valid frame rather than at EOF. */

    res = write_log_header();
    if (res != FR_OK) {
        sd_fault("writing log header", res);
        return;
    }

    printf("logging to %s\n\r", name);
    have_log_index = true;
    logData.ready  = true;
    prev_record    = logData.record;
    SensorData_Buffer_Reset_Dropped(&logData);
    BSP_LED_Off(LED_RED);
    state = SdState::Logging;
}

static void sd_reopen_file()
{
    char name[32];
    snprintf(name, sizeof(name), "log%04u.bin", log_index);

    FRESULT res = f_open(&Fil, name, FA_OPEN_APPEND | FA_WRITE);
    if (res != FR_OK) {
        have_log_index = false;      /* give up on it; take a fresh name */
        return;
    }

    FSIZE_t sz = f_size(&Fil);
    if (sz < LOG_HEADER_BYTES) {     /* header never completed - unusable */
        f_close(&Fil);
        have_log_index = false;
        return;
    }

    FSIZE_t aligned = LOG_HEADER_BYTES
                    + ((sz - LOG_HEADER_BYTES) / sizeof(SensorData_t))
                      * sizeof(SensorData_t);

    if (aligned != sz) {
        if (f_lseek(&Fil, aligned) != FR_OK || f_truncate(&Fil) != FR_OK) {
            sd_fault("trimming partial record", FR_INT_ERR);
            return;
        }
    }

    printf("resumed %s at %lu bytes\n\r", name, (unsigned long)aligned);
    logData.ready = true;
    prev_record   = logData.record;
    BSP_LED_Off(LED_RED);
    state = SdState::Logging;
}

/* ------------------------------------------------------------------ */
/* Tail flush at end of recording                                      */
/* ------------------------------------------------------------------ */

static void sd_flush_tail()
{
    const SensorData_t *tail = NULL;
    size_t n = SensorData_Buffer_ClaimTail(&logData, &tail);

    if (n) {
        UINT    wc  = 0;
        FRESULT res = f_write(&Fil, tail, n * sizeof(SensorData_t), &wc);

        if (res != FR_OK || wc != n * sizeof(SensorData_t)) {
            sd_fault("writing tail", res);
            return;
        }
        SensorData_Buffer_ReleaseTail(&logData, n);
        printf("flushed %u trailing records\n\r", (unsigned)n);
    }

    if (f_sync(&Fil) != FR_OK) {
        sd_fault("syncing tail", FR_DISK_ERR);
    }
}

/* ------------------------------------------------------------------ */
/* Public entry points                                                 */
/* ------------------------------------------------------------------ */

void sd_card_prep()
{
    switch (state) {

    case SdState::NotMounted:
        sd_try_mount();
        break;

    case SdState::NoFile:
        if (have_log_index) sd_reopen_file();
        else                sd_create_file();
        break;

    case SdState::Logging:
        if (tail_armed) {
            sd_flush_tail();
            tail_armed = false;
            if (state == SdState::Logging) {
                logData.ready = true;         /* re-arm for another run */
            }
        } else if (prev_record && !logData.record) {
            logData.ready = false;            /* stop the producer now  */
            tail_armed    = true;             /* read the tail next tick */
        }
        prev_record = logData.record;
        break;
    }
}

void sd_card_task_function()
{
    if (state != SdState::Logging) {
        return;
    }

    size_t n = 0;
    const SensorData_t *chunk = SensorData_Buffer_ClaimHalf(&logData, &n);
    if (n == 0) {
        return;
    }

    sd_card_profiler.start();

    UINT    wc  = 0;
    FRESULT res = f_write(&Fil, chunk, n * sizeof(SensorData_t), &wc);

    /* Release before anything else can fail: a half left claimed would stall
     * the producer permanently. Losing this half's data beats losing all of it. */
    SensorData_Buffer_ReleaseHalf(&logData);

    if (res != FR_OK || wc != n * sizeof(SensorData_t)) {
        sd_card_profiler.end();
        sd_fault("writing half", res);
        return;
    }

    res = f_sync(&Fil);
    if (res != FR_OK) {
        sd_card_profiler.end();
        sd_fault("f_sync", res);
        return;
    }

    BSP_LED_Off(LED_RED);
    sd_card_profiler.end();
}
