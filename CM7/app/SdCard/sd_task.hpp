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
    UsbOwned,     /* card handed to USB MSC; sd_card_prep() stands down */
};

/* Slow path. Call on a timer (~500 ms): mounts, opens files, recovers from
 * faults, and flushes the trailing partial half when recording stops. */
void sd_card_prep();

/* Fast path. Call every main-loop iteration: writes one full half if one is
 * ready, otherwise returns immediately. */
void sd_card_task_function();

SdState sd_card_state();

/* Cleanly closes/truncates whatever's open (usually just the near-empty
 * background file) and unmounts FatFs, then moves to SdState::UsbOwned so
 * sd_card_prep()/sd_card_task_function() stand down and stop touching the
 * card. One-way for this boot: there is no matching "give it back" call --
 * resuming normal logging after a USB session requires a reset, since the
 * in-memory FatFs state can't be trusted once a host has been writing raw
 * blocks to the card. Touches FatFs, so app_loop() context only, same as
 * the rest of this module. */
void sd_release_for_usb();

#endif /* SD_TASK_HPP_ */
