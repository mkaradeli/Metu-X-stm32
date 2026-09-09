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
#include <stdio.h>

static volatile bool usb_msc_requested_flag = false;
static volatile bool usb_msc_ready_flag     = false;

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
