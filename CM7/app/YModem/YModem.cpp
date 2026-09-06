/*
 * YModem.cpp
 *
 *  Created on: Sep 7, 2026
 *      Author: karadeli
 *
 *  Non-blocking YMODEM (1K/CRC-16) sender, polled from app_loop(). Classic
 *  XMODEM/YMODEM senders block on each response with a multi-second timeout;
 *  this firmware has no room for that (same main loop also drives actuator
 *  control and SD logging), so every wait is a state + a HAL_GetTick()
 *  deadline instead.
 *
 *  Context split (do not move work across this line):
 *    - ymodem_request_transfer() and ymodem_rx_byte() run from ISR context
 *      (TIM7, via MissionUart's raw sink / MissionControl::HandleCommand()).
 *      They must stay cheap: no FatFs, no blocking.
 *    - ymodem_poll() runs from app_loop() (the base-level main loop). All
 *      FatFs calls live here -- FatFs has no reentrancy support
 *      (ffconf.h: _FS_REENTRANT=0) and everywhere else it's only ever
 *      touched from this same context (sd_card_prep()/sd_card_task_function()).
 *
 *  TX reuses the existing common_print_buffer ring (rb_write()/rb_flush()) --
 *  already ISR-and-task safe and already DMA-drained every app_loop()
 *  iteration, so this never needs to own USART3's TX DMA directly.
 *
 *  If ENABLE_PRINT is ever turned on, printf() output would interleave with
 *  this binary stream on the same ring buffer and corrupt the transfer --
 *  the two are only safe together while ENABLE_PRINT stays false.
 */

#include "YModem.hpp"
#include "MissionUart.hpp"
#include "MissionControl.hpp"
#include "../SdCard/sd_task.hpp"
#include "ring_buffer.h"
#include "fatfs.h"
#include <string.h>
#include <stdio.h>

#define YM_SOH 0x01u
#define YM_STX 0x02u
#define YM_EOT 0x04u
#define YM_ACK 0x06u
#define YM_NAK 0x15u
#define YM_CAN 0x18u
#define YM_C   0x43u
#define YM_SUB 0x1Au

#define YM_HDR_SIZE             128u
#define YM_BLOCK_SIZE          1024u
#define YM_MAX_RETRIES           10u
#define YM_BLOCK_TIMEOUT_MS    3000u
#define YM_INITIAL_C_TIMEOUT_MS 60000u

enum class YmState : uint8_t {
	Idle = 0,
	StartRequested,
	WaitInitialC,
	WaitHeaderResp,
	SendData,
	WaitDataResp,
	WaitEot1Resp,
	WaitEot2Resp,
	WaitFinalC,
	WaitFinalResp,
};

/* ---- ISR -> poll byte queue (mirrors rb_push()'s critical-section pattern,
 * CPU-to-CPU only -- no DMA touches this, so plain RAM is fine). ---- */
#define YM_RXQ_SIZE 8u
static volatile uint8_t ym_rxq[YM_RXQ_SIZE];
static volatile uint8_t ym_rxq_head = 0;
static volatile uint8_t ym_rxq_tail = 0;

void ymodem_rx_byte(uint8_t c)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	uint8_t next = (uint8_t)((ym_rxq_head + 1u) % YM_RXQ_SIZE);
	if (next != ym_rxq_tail) {
		ym_rxq[ym_rxq_head] = c;
		ym_rxq_head = next;
	}

	__set_PRIMASK(primask);
}

static bool ym_rxq_pop(uint8_t *out)
{
	if (ym_rxq_head == ym_rxq_tail) return false;

	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	*out = ym_rxq[ym_rxq_tail];
	ym_rxq_tail = (uint8_t)((ym_rxq_tail + 1u) % YM_RXQ_SIZE);

	__set_PRIMASK(primask);
	return true;
}

static void ym_rxq_clear()
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	ym_rxq_head = ym_rxq_tail = 0;
	__set_PRIMASK(primask);
}

/* ---- state, app_loop() context only ---- */
static volatile YmState ym_state = YmState::Idle;
static FIL      ym_fil;
static char     ym_name[32];
static uint32_t ym_size;
static uint8_t  ym_block_num;
static uint8_t  ym_retries;
static uint8_t  ym_can_count;
static uint32_t ym_deadline;

/* SOH/STX + block# + ~block# + up to 1024B payload + 2B CRC. CPU-only, never
 * touched by DMA (rb_write() memcpy's it into common_print_buffer), so no
 * SRAM3/alignment requirement like the DMA-visible buffers elsewhere. */
static uint8_t  ym_block_buf[3 + YM_BLOCK_SIZE + 2];
static uint32_t ym_block_len;

bool ymodem_active()
{
	return ym_state != YmState::Idle;
}

/* CRC-16/XMODEM: poly 0x1021, init 0x0000, no reflection. Deliberately not
 * crc16_calc() -- that drives the shared hardware CRC unit configured with
 * init 0xFFFF (CRC-16/CCITT-FALSE) for SD log records, a different variant
 * that also isn't safe to reprogram out from under the logger. */
static uint16_t ym_crc16(const uint8_t *p, size_t n)
{
	uint16_t crc = 0x0000;
	while (n--) {
		crc ^= (uint16_t)(*p++) << 8;
		for (int i = 0; i < 8; i++) {
			crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
		}
	}
	return crc;
}

static void ym_send(const uint8_t *data, size_t n)
{
	rb_write(&common_print_buffer, data, n);
}

static void ym_send_byte(uint8_t b)
{
	rb_write(&common_print_buffer, &b, 1);
}

static void ym_arm_timeout(uint32_t ms)
{
	ym_deadline = HAL_GetTick() + ms;
}

static bool ym_timed_out()
{
	return (int32_t)(HAL_GetTick() - ym_deadline) >= 0;
}

static void ym_cleanup()
{
	f_close(&ym_fil);
	mission_uart_exit_raw();
	ym_rxq_clear();
	ym_state = YmState::Idle;
}

static void ym_abort()
{
	uint8_t can2[2] = { YM_CAN, YM_CAN };
	ym_send(can2, 2);
	ym_cleanup();
}

/* Block 0 (filename+size header) or, with terminator=true, the all-zero
 * batch-terminator block that tells the receiver no more files are coming. */
static void ym_build_header_block(bool terminator)
{
	uint8_t *p = ym_block_buf;
	p[0] = YM_SOH;
	p[1] = 0x00u;
	p[2] = 0xFFu;
	memset(p + 3, 0, YM_HDR_SIZE);

	if (!terminator) {
		size_t nlen = strlen(ym_name);
		memcpy(p + 3, ym_name, nlen);
		char sizebuf[12];
		int slen = snprintf(sizebuf, sizeof(sizebuf), "%lu", (unsigned long)ym_size);
		if (slen > 0) memcpy(p + 3 + nlen + 1, sizebuf, (size_t)slen);
	}

	uint16_t crc = ym_crc16(p + 3, YM_HDR_SIZE);
	p[3 + YM_HDR_SIZE]     = (uint8_t)(crc >> 8);
	p[3 + YM_HDR_SIZE + 1] = (uint8_t)(crc & 0xFFu);
	ym_block_len = 3 + YM_HDR_SIZE + 2;
}

/* Reads up to 1024B from the file into an STX data block, 0x1A-padding a
 * short final read. Returns false at EOF (nothing left to send). */
static bool ym_build_data_block(uint8_t blk)
{
	UINT     br      = 0;
	uint8_t *payload = ym_block_buf + 3;

	FRESULT res = f_read(&ym_fil, payload, YM_BLOCK_SIZE, &br);
	if (res != FR_OK || br == 0) return false;

	if (br < YM_BLOCK_SIZE) {
		memset(payload + br, YM_SUB, YM_BLOCK_SIZE - br);
	}

	ym_block_buf[0] = YM_STX;
	ym_block_buf[1] = blk;
	ym_block_buf[2] = (uint8_t)(0xFFu - blk);

	uint16_t crc = ym_crc16(payload, YM_BLOCK_SIZE);
	ym_block_buf[3 + YM_BLOCK_SIZE]     = (uint8_t)(crc >> 8);
	ym_block_buf[3 + YM_BLOCK_SIZE + 1] = (uint8_t)(crc & 0xFFu);
	ym_block_len = 3 + YM_BLOCK_SIZE + 2;
	return true;
}

/* ------------------------------------------------------------------ */

bool ymodem_request_transfer(char *reply, size_t n)
{
	if (ym_state != YmState::Idle) {
		snprintf(reply, n, "GETLOG FAIL: transfer already in progress\r\n");
		return false;
	}
	if (missionControl.system_mode != system_modes::IDLE || missionControl.running) {
		snprintf(reply, n, "GETLOG FAIL: not idle\r\n");
		return false;
	}
	if (sd_card_state() != SdState::NoFile) {
		snprintf(reply, n, "GETLOG FAIL: SD card busy or not mounted\r\n");
		return false;
	}

	ym_state = YmState::StartRequested;
	snprintf(reply, n, "GETLOG: preparing transfer\r\n");
	return true;
}

void ymodem_poll()
{
	uint8_t c;

	switch (ym_state) {

	case YmState::Idle:
		return;

	case YmState::StartRequested: {
		if (!sd_get_last_log_name(ym_name, sizeof(ym_name))) {
			ym_state = YmState::Idle;    /* nothing logged yet */
			return;
		}
		if (f_open(&ym_fil, ym_name, FA_OPEN_EXISTING | FA_READ) != FR_OK) {
			ym_state = YmState::Idle;
			return;
		}
		ym_size      = (uint32_t)f_size(&ym_fil);
		ym_block_num = 1;
		ym_retries   = 0;
		ym_can_count = 0;

		rb_drain_blocking(200);          /* quiesce any tail of the 500Hz stream */
		ym_rxq_clear();
		mission_uart_enter_raw(ymodem_rx_byte);

		ym_arm_timeout(YM_INITIAL_C_TIMEOUT_MS);
		ym_state = YmState::WaitInitialC;
		break;
	}

	case YmState::WaitInitialC:
		if (ym_rxq_pop(&c)) {
			if (c == YM_C) {
				ym_build_header_block(false);
				ym_send(ym_block_buf, ym_block_len);
				ym_retries = 0;
				ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
				ym_state = YmState::WaitHeaderResp;
			}
			/* anything else while waiting for the handshake: ignore */
		} else if (ym_timed_out()) {
			ym_cleanup();                /* no receiver showed up */
		}
		break;

	case YmState::WaitHeaderResp:
		if (ym_rxq_pop(&c)) {
			if (c == YM_ACK) {
				ym_can_count = 0;
				ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
				ym_state = YmState::SendData;
			} else if (c == YM_NAK) {
				ym_can_count = 0;
				if (++ym_retries > YM_MAX_RETRIES) { ym_abort(); break; }
				ym_send(ym_block_buf, ym_block_len);
				ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
			} else if (c == YM_CAN) {
				if (++ym_can_count >= 2) ym_cleanup();
			} else {
				ym_can_count = 0;         /* stray byte, ignore */
			}
		} else if (ym_timed_out()) {
			if (++ym_retries > YM_MAX_RETRIES) { ym_abort(); break; }
			ym_send(ym_block_buf, ym_block_len);
			ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
		}
		break;

	case YmState::SendData:
		/* Some receivers re-send 'C' right after ACKing block 0 to reconfirm
		 * CRC mode; discard anything pending here rather than misreading it
		 * as a NAK against the next data block. */
		(void)ym_rxq_pop(&c);

		if (!ym_build_data_block(ym_block_num)) {
			ym_send_byte(YM_EOT);
			ym_retries = 0;
			ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
			ym_state = YmState::WaitEot1Resp;
			break;
		}
		ym_send(ym_block_buf, ym_block_len);
		ym_retries = 0;
		ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
		ym_state = YmState::WaitDataResp;
		break;

	case YmState::WaitDataResp:
		if (ym_rxq_pop(&c)) {
			if (c == YM_ACK) {
				ym_block_num++;
				ym_can_count = 0;
				ym_state = YmState::SendData;
			} else if (c == YM_NAK) {
				ym_can_count = 0;
				if (++ym_retries > YM_MAX_RETRIES) { ym_abort(); break; }
				ym_send(ym_block_buf, ym_block_len);
				ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
			} else if (c == YM_CAN) {
				if (++ym_can_count >= 2) ym_cleanup();
			} else {
				ym_can_count = 0;
			}
		} else if (ym_timed_out()) {
			if (++ym_retries > YM_MAX_RETRIES) { ym_abort(); break; }
			ym_send(ym_block_buf, ym_block_len);
			ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
		}
		break;

	/* Classic quirk: some receivers NAK the first EOT and ACK the second.
	 * The file's data is already fully sent by this point either way, so
	 * nothing here is allowed to hard-fail the transfer. */
	case YmState::WaitEot1Resp:
		if (ym_rxq_pop(&c)) {
			if (c == YM_NAK) {
				ym_send_byte(YM_EOT);
				ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
				ym_state = YmState::WaitEot2Resp;
			} else if (c == YM_ACK) {
				ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
				ym_state = YmState::WaitFinalC;
			}
		} else if (ym_timed_out()) {
			ym_send_byte(YM_EOT);
			ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
			ym_state = YmState::WaitEot2Resp;
		}
		break;

	case YmState::WaitEot2Resp:
		if (ym_rxq_pop(&c) || ym_timed_out()) {
			ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
			ym_state = YmState::WaitFinalC;
		}
		break;

	case YmState::WaitFinalC:
		if (ym_rxq_pop(&c)) {
			if (c == YM_C) {
				ym_build_header_block(true);
				ym_send(ym_block_buf, ym_block_len);
				ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
				ym_state = YmState::WaitFinalResp;
			}
		} else if (ym_timed_out()) {
			ym_cleanup();                /* receiver didn't ask for more -- done */
		}
		break;

	case YmState::WaitFinalResp:
		if (ym_rxq_pop(&c) || ym_timed_out()) {
			ym_cleanup();                /* file already transferred either way */
		}
		break;
	}
}
