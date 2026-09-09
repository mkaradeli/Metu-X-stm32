/*
 * UsbMsc.cpp
 *
 *  Created on: Sep 9, 2026
 *      Author: karadeli
 *
 *  Context split (do not move work across this line -- same reasoning as
 *  YModem.cpp):
 *    - usb_msc_request() runs from ISR context (TIM7, via
 *      MissionControl::HandleCommand()). Must stay cheap: no FatFs.
 *    - usb_msc_poll() runs from app_loop() (the base-level main loop). All
 *      FatFs calls (sd_release_for_usb()) live here.
 */

#include "UsbMsc.hpp"
#include "MissionControl.hpp"
#include "../SdCard/sd_task.hpp"
#include "../YModem/YModem.hpp"
#include "main.h"
#include <stdio.h>

static volatile bool usb_msc_requested_flag = false;
static volatile bool usb_msc_ready_flag     = false;

/* Auto-detect: OTG_FS VBUS present (a real cable into a powered host on the
 * OTG_FS connector -- physically separate from the ST-LINK/UART USB port)
 * is treated the same as typing USBMSC by hand. Debounced and throttled
 * since the resulting move is one-way (EnterUsbMode() locks the mission
 * controller for the rest of the boot) -- a VBUS glitch or a cable plugged
 * in mid-mission must not trip it. */
static bool     vbus_was_present      = false;
static uint32_t vbus_present_since_ms = 0;
static uint32_t vbus_last_attempt_ms  = 0;
static const uint32_t VBUS_DEBOUNCE_MS = 200;   /* connector-mating bounce */
static const uint32_t VBUS_RETRY_MS    = 1000;  /* re-check while not-idle */

static bool usb_vbus_present()
{
	return (USB_OTG_FS->GOTGCTL & USB_OTG_GOTGCTL_BSESVLD) != 0;
}

static void usb_msc_check_vbus_auto()
{
	if (usb_msc_requested_flag || usb_msc_ready_flag) return;

	if (!usb_vbus_present()) {
		vbus_was_present = false;
		return;
	}
	if (!vbus_was_present) {
		vbus_was_present      = true;
		vbus_present_since_ms = uwTick;
		vbus_last_attempt_ms  = vbus_present_since_ms - VBUS_RETRY_MS;
	}
	if ((uwTick - vbus_present_since_ms) < VBUS_DEBOUNCE_MS) return;
	if ((uwTick - vbus_last_attempt_ms) < VBUS_RETRY_MS) return;
	vbus_last_attempt_ms = uwTick;

	char reply[80];
	if (usb_msc_request(reply, sizeof(reply))) {
		printf("USB MSC: cable detected -- %s", reply);
	}
	/* refused (busy / not idle): stay quiet and retry every VBUS_RETRY_MS
	 * for as long as the cable stays plugged in, no console spam */
}

bool usb_msc_request(char *reply, size_t n)
{
	if (usb_msc_requested_flag || usb_msc_ready_flag) {
		snprintf(reply, n, "USBMSC: already active\r\n");
		return false;
	}
	if (ymodem_active()) {
		snprintf(reply, n, "USBMSC FAIL: log transfer in progress\r\n");
		return false;
	}
	if (missionControl.system_mode != system_modes::IDLE || missionControl.running) {
		snprintf(reply, n, "USBMSC FAIL: not idle\r\n");
		return false;
	}

	usb_msc_requested_flag = true;
	snprintf(reply, n, "USBMSC: handing SD card to USB -- reset the board "
	                    "to resume normal logging\r\n");
	return true;
}

void usb_msc_poll()
{
	usb_msc_check_vbus_auto();

	if (!usb_msc_requested_flag) return;
	usb_msc_requested_flag = false;

	sd_release_for_usb();      /* app_loop() context -- safe to touch FatFs here */
	missionControl.EnterUsbMode();   /* locks system_mode -- see MissionControl.cpp */
	usb_msc_ready_flag = true;
	printf("USB MSC: SD card released, mount it from the host now\n\r");
}

bool usb_msc_ready()
{
	return usb_msc_ready_flag;
}

bool usb_msc_active()
{
	return usb_msc_requested_flag || usb_msc_ready_flag;
}
