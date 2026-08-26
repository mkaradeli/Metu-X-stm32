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
 *
 * Mission binding
 * ---------------
 * A file belongs to exactly one mission. It is created ahead of time, while
 * the vehicle is idle, using the header text and the duration estimate of the
 * *selected* mission - f_expand of tens of MB is far too slow to do on the
 * trigger. Consequences:
 *   - selecting another mission over UART while idle discards the empty file
 *     and opens a fresh one for the new mission;
 *   - the end of a run closes the file and rolls to a new one, so one run is
 *     always one file, never an append onto the previous mission's data.
 *
 * Because f_expand sets the file size to the full extent up front, the true
 * end of data is the write pointer, not f_size. Two places care:
 *   - sd_finalize_file() truncates back to the write pointer on close, so the
 *     file on the card is exactly as long as the data in it;
 *   - sd_reopen_file() cannot trust f_size after a fault, so it bisects for
 *     the last written record instead (see find_resume_offset).
 */

#include <shared_memory.h>
#include "sd_task.hpp"
#include "fatfs.h"
#include "ff_gen_drv.h"
#include "globals.hpp"
#include "MissionControl.hpp"
#include <string.h>
#include <stdio.h>

extern Profiler sd_card_profiler;
extern Disk_drvTypeDef disk;

/* Records are logged once per pressure-loop tick. Only used to size the
 * pre-allocation, a wrong value costs disk space, not data. */
#define LOG_RATE_HZ        1000u
/* Header text is length-prefixed with a uint8_t and has to fit the block. */
#define LOG_TEXT_MAX       200u
/* Refuse absurd pre-allocations (bad mission table entry). */
#define LOG_PREALLOC_MAX   (512u * 1024u * 1024u)

/* ------------------------------------------------------------------ */
/* State                                                               */
/* ------------------------------------------------------------------ */

static FATFS   FatFs;
static FIL     Fil;
static SdState state = SdState::NotMounted;

static uint16_t log_index      = 0;
static bool     have_log_index = false;   /* reopen this index after a fault */
static char     log_name[32]   = { 0 };

/* Mission this file was created for. 0xFF = none bound yet. */
static uint8_t  bound_mission  = 0xFFu;

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

/* "<mission name>: <mission header>", NUL included in the returned length,
 * matching the old logHeaderSize == sizeof(logHeader) convention so the
 * parser needs no change. */
static uint8_t build_header_text(char *out, size_t n)
{
    int len = snprintf(out, n, "%s: %s",
                       missionControl.SelectedName(),
                       missionControl.SelectedHeader());

    if (len < 0)            len = 0;
    if ((size_t)len >= n)   len = (int)n - 1;        /* snprintf truncated */

    return (uint8_t)(len + 1);                       /* include the NUL */
}

static FRESULT write_log_header()
{
    uint8_t hdr[LOG_HEADER_BYTES] = { 0 };
    UINT    wc = 0;
    size_t  o  = 0;

    char    text[LOG_TEXT_MAX];
    uint8_t textSize = build_header_text(text, sizeof(text));

    memcpy(hdr + o, &logFormatId,      sizeof(uint16_t)); o += sizeof(uint16_t);
    memcpy(hdr + o, &textSize,         sizeof(uint8_t));  o += sizeof(uint8_t);
    memcpy(hdr + o, text,              textSize);         o += textSize;
    memcpy(hdr + o, &sensorDataLength, sizeof(uint16_t));

    FRESULT res = f_write(&Fil, hdr, sizeof(hdr), &wc);
    if (res == FR_OK && wc != sizeof(hdr)) {
        res = FR_INT_ERR;            /* short header => misaligned records */
    }
    return res;
}

/* Bytes to reserve for the selected mission. 0 = do not pre-allocate. */
static uint32_t prealloc_bytes()
{
    uint32_t ms = missionControl.LogPreallocMs();
    if (ms == 0) return 0;

    uint64_t bytes = (uint64_t)ms * LOG_RATE_HZ / 1000ull * sizeof(SensorData_t);
    bytes += LOG_HEADER_BYTES;

    if (bytes > LOG_PREALLOC_MAX) bytes = LOG_PREALLOC_MAX;
    return (uint32_t)bytes;
}

static void sd_create_file()
{
    if (!find_free_log_index(&log_index)) {
        sd_fault("scanning for free log index", FR_DISK_ERR);
        return;
    }

    snprintf(log_name, sizeof(log_name), "log%04u.bin", log_index);

    FRESULT res = f_open(&Fil, log_name, FA_CREATE_NEW | FA_WRITE | FA_READ);
    if (res != FR_OK) {
        sd_fault("creating log file", res);
        return;
    }

    /* Contiguous pre-allocation makes f_sync cheap. It also sets the file
     * size to the full extent immediately, which is why the write pointer,
     * not f_size, is the end of data everywhere below. Not fatal if the card
     * has no contiguous run that big: fall back to plain appending. */
    const uint32_t want = prealloc_bytes();
    if (want) {
        res = f_expand(&Fil, want, 1);
        if (res != FR_OK) {
            printf("f_expand %lu B failed (res=%d), logging unallocated\n\r",
                   (unsigned long)want, (int)res);
        }
    }

    res = write_log_header();
    if (res != FR_OK) {
        sd_fault("writing log header", res);
        return;
    }

    bound_mission = missionControl.SelectedIndex();

    printf("logging to %s, mission %u:%s, reserved %lu B\n\r",
           log_name, (unsigned)bound_mission, missionControl.SelectedName(),
           (unsigned long)want);

    have_log_index = true;
    logData.ready  = true;
    prev_record    = logData.record;
    SensorData_Buffer_Reset_Dropped(&logData);
    BSP_LED_Off(LED_RED);
    state = SdState::Logging;
}

/* End of data in a pre-allocated file, found by bisection on the record
 * grid: the unwritten extent is zero-filled, so the 'K','D' sync bytes at
 * the head of each record are present exactly up to the last write. Also
 * correct for files opened without f_expand, where it lands on f_size. */
static bool find_resume_offset(FSIZE_t *out)
{
    const FSIZE_t rec = sizeof(SensorData_t);
    FSIZE_t sz = f_size(&Fil);

    if (sz < LOG_HEADER_BYTES) return false;

    uint32_t lo = 0;                                       /* record index */
    uint32_t hi = (uint32_t)((sz - LOG_HEADER_BYTES) / rec);

    while (lo < hi) {
        uint32_t mid = lo + ((hi - lo) >> 1);
        uint8_t  magic[2] = { 0, 0 };
        UINT     rc = 0;

        if (f_lseek(&Fil, LOG_HEADER_BYTES + (FSIZE_t)mid * rec) != FR_OK)
            return false;
        if (f_read(&Fil, magic, sizeof(magic), &rc) != FR_OK || rc != sizeof(magic))
            return false;

        if (magic[0] == 'K' && magic[1] == 'D') lo = mid + 1;   /* written  */
        else                                    hi = mid;       /* blank    */
    }

    *out = LOG_HEADER_BYTES + (FSIZE_t)lo * rec;
    return true;
}

static void sd_reopen_file()
{
    FRESULT res = f_open(&Fil, log_name, FA_OPEN_EXISTING | FA_WRITE | FA_READ);
    if (res != FR_OK) {
        have_log_index = false;      /* give up on it; take a fresh name */
        return;
    }

    if (f_size(&Fil) < LOG_HEADER_BYTES) {   /* header never completed */
        f_close(&Fil);
        have_log_index = false;
        return;
    }

    FSIZE_t resume = 0;
    if (!find_resume_offset(&resume)) {
        f_close(&Fil);
        have_log_index = false;
        return;
    }

    if (f_lseek(&Fil, resume) != FR_OK) {
        sd_fault("seeking to resume point", FR_INT_ERR);
        return;
    }

    printf("resumed %s at %lu bytes\n\r", log_name, (unsigned long)resume);
    logData.ready = true;
    prev_record   = logData.record;
    BSP_LED_Off(LED_RED);
    state = SdState::Logging;
}

/* Close the current file and go back to NoFile so the next prep tick opens a
 * fresh one for whatever mission is selected then. Trims the unused tail of
 * the pre-allocated extent. An untouched file (header only) is deleted rather
 * than left behind as a stub. */
static void sd_finalize_file()
{
    FSIZE_t end   = f_tell(&Fil);
    bool    empty = (end <= LOG_HEADER_BYTES);

    if (f_truncate(&Fil) != FR_OK) {
        printf("could not trim %s to %lu B\n\r", log_name, (unsigned long)end);
    }
    f_close(&Fil);

    if (empty) f_unlink(log_name);
    else       printf("closed %s at %lu bytes\n\r", log_name, (unsigned long)end);

    logData.ready  = false;
    have_log_index = false;
    bound_mission  = 0xFFu;
    tail_armed     = false;
    state          = SdState::NoFile;
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
                sd_finalize_file();               /* one run, one file */
            }
        } else if (prev_record && !logData.record) {
            logData.ready = false;                /* stop the producer now  */
            tail_armed    = true;                 /* read the tail next tick */
        } else if (!logData.record
                   && bound_mission != missionControl.SelectedIndex()) {
            sd_finalize_file();                   /* mission changed while idle */
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
