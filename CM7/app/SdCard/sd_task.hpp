/*
 * sd_task.hpp
 *
 *  Created on: Aug 2, 2026
 *      Author: karadeli
 */

#ifndef SD_TASK_HPP_
#define SD_TASK_HPP_

#include <stdint.h>
#include <stddef.h>

/* Every log file starts with a fixed 512-byte header block so that the record
 * stream begins on a sector boundary. The parser depends on this constant. */
#define LOG_HEADER_BYTES 512u

enum class SdState : uint8_t {
    NotMounted,   /* no filesystem: retry f_mount()                     */
    NoFile,       /* mounted, but no log file open: create or reopen    */
    Logging,      /* file open, flushing full halves as they appear     */
};

/* Slow path. Call on a timer (~500 ms): mounts, opens files, recovers from
 * faults, and flushes the trailing partial half when recording stops. */
void sd_card_prep();

/* Fast path. Call every main-loop iteration: writes one full half if one is
 * ready, otherwise returns immediately. */
void sd_card_task_function();

SdState sd_card_state();

/* Independent bisection lookup for the most recently written log file
 * (mirrors find_free_log_index()'s technique without touching the write
 * path). Only meaningful, and only safe to call, while idle
 * (sd_card_state() == SdState::NoFile) from app_loop() context -- FatFs has
 * no reentrancy support (_FS_REENTRANT=0), so this must never be called from
 * an ISR. Returns false if no log has ever been written or the card is
 * unreadable. */
bool sd_get_last_log_name(char *out, size_t outsz);

#endif /* SD_TASK_HPP_ */
