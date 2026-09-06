/*
 * YModem.hpp
 *
 *  Created on: Sep 7, 2026
 *      Author: karadeli
 *
 *  GETLOG: sends the most recent SD log file to the ground station over
 *  USART3 using YMODEM (1K blocks, CRC-16), pausing the 500Hz telemetry push
 *  and taking over the console UART's RX for the duration.
 *
 *  Ground-only feature: transfers only ever happen pre-flight, so
 *  MissionControl::Start() simply refuses to arm while a transfer is active
 *  rather than trying to keep the console reachable mid-transfer.
 */

#ifndef YMODEM_YMODEM_HPP_
#define YMODEM_YMODEM_HPP_

#include <stdint.h>
#include <stddef.h>

/* Called from MissionControl::HandleCommand() (TIM7 ISR context, same as
 * every other console verb today). Only cheap state checks -- no FatFs,
 * nothing blocking. Fills `reply` either way, mirroring the other verbs. */
bool ymodem_request_transfer(char *reply, size_t n);

/* Call once per app_loop() iteration. This is the ONLY context allowed to
 * touch FatFs for this feature (open/read the log file) -- FatFs has no
 * reentrancy support (_FS_REENTRANT=0) and is otherwise only ever called
 * from app_loop()'s sd_card_prep()/sd_card_task_function(). */
void ymodem_poll();

/* Registered as MissionUart's raw RX sink; invoked from the TIM7 ISR while a
 * transfer is active. Stays tiny: pushes into a small byte queue for
 * ymodem_poll() to drain, nothing else. */
void ymodem_rx_byte(uint8_t c);

/* True from the moment a transfer is requested until it finishes, errors
 * out, or is cancelled. Gates the 500Hz telemetry rb_write() and
 * MissionControl::Start(). */
bool ymodem_active();

#endif /* YMODEM_YMODEM_HPP_ */
