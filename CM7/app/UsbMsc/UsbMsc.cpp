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

/* Auto-detect: NOT VBUS-based. SB21 ON (this board's shipped default, per
 * UM2408 Table 13, correct for wiring PA9 to CN13's actual VBUS pin) still
 * left USB_OTG_FS->GOTGCTL.BSESVLD reading "present" with nothing in CN13
 * at all -- with no cable, PA9/VBUS-sense floats (no pull-down on this
 * net), and the OTG core's analog comparator reads that float as valid
 * often enough to be useless as a signal. No jumper fixes that.
 *
 * Instead, trigger off actual enumeration: usb_msc_notify_configured() is
 * called from STORAGE_Init_FS() (usbd_storage_if.c), which only runs when
 * a real host sends SET_CONFIGURATION -- traced through MSC_BOT_Init() /
 * USBD_MSC_Init() / USBD_SetConfig(). That requires genuine host traffic
 * on D+/D-, which a floating sense pin cannot fake. Runs from USB-stack
 * (ISR) context, so it only sets a flag -- same cheap-context rule as
 * usb_msc_request(). */
static volatile bool usb_msc_host_configured = false;

static bool     usb_msc_attempted_once  = false;
static uint32_t usb_msc_last_attempt_ms = 0;
static const uint32_t USB_MSC_RETRY_MS = 1000;   /* re-check while not-idle */

void usb_msc_notify_configured()
{
	usb_msc_host_configured = true;
}

static void usb_msc_check_configured_auto()
{
	if (usb_msc_requested_flag || usb_msc_ready_flag) return;
	if (!usb_msc_host_configured) return;
	if (usb_msc_attempted_once && (uwTick - usb_msc_last_attempt_ms) < USB_MSC_RETRY_MS) return;

	usb_msc_attempted_once  = true;
	usb_msc_last_attempt_ms = uwTick;

	char reply[80];
	if (usb_msc_request(reply, sizeof(reply))) {
		printf("USB MSC: host enumerated the device -- %s", reply);
	}
	/* refused (busy / not idle): stay quiet and retry every
	 * USB_MSC_RETRY_MS for as long as the host stays configured, no
	 * console spam */
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
	usb_msc_check_configured_auto();

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
