/*
 * UsbMsc.hpp
 *
 *  Created on: Sep 9, 2026
 *      Author: karadeli
 *
 *  USBMSC: hands the SD card over to the USB host as a Mass Storage device,
 *  so any log file can be pulled off directly (drag-and-drop in Finder/
 *  Explorer) instead of one-at-a-time over GETLOG/YMODEM.
 *
 *  Entered either by the console USBMSC command or automatically when the
 *  OTG_FS connector's VBUS goes live (see usb_msc_check_vbus_auto() in the
 *  .cpp) -- both funnel through usb_msc_request(). Auto-entry waits for the
 *  vehicle to be idle rather than interrupting a mission, and only latches
 *  once VBUS has been stably present for a short debounce window.
 *
 *  One-way for the boot it's used in -- see sd_release_for_usb() in
 *  sd_task.hpp. Resuming normal flight logging needs a reset.
 */

#ifndef USBMSC_USBMSC_HPP_
#define USBMSC_USBMSC_HPP_

#include <stddef.h>
#include <stdbool.h>

/* usbd_storage_if.c (CubeMX-generated, plain C) needs to call
 * usb_msc_ready() directly -- give these C linkage so that works. */
#ifdef __cplusplus
extern "C" {
#endif

/* Called from MissionControl::HandleCommand() (TIM7 ISR context, same as
 * every other console verb). Only cheap state checks -- no FatFs, nothing
 * blocking. The actual card release happens later from usb_msc_poll(). */
bool usb_msc_request(char *reply, size_t n);

/* Call once per app_loop() iteration. This is the only context allowed to
 * touch FatFs for this feature (sd_release_for_usb()) -- see the same
 * constraint already documented for ymodem_poll() in YModem.hpp. */
void usb_msc_poll();

/* True once the SD card has actually been handed to the USB host --
 * usbd_storage_if.c's STORAGE_IsReady_FS() gates on this so the host sees
 * a normal "no media" state, not a live filesystem race, until then. */
bool usb_msc_ready();

/* True from the moment USBMSC is requested (before usb_msc_poll() has even
 * run) through the rest of this boot -- broader than usb_msc_ready(), used
 * to gate MissionControl::Start() so arming can't race the card handover. */
bool usb_msc_active();

#ifdef __cplusplus
}
#endif

#endif /* USBMSC_USBMSC_HPP_ */
