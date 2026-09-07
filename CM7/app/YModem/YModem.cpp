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
#define YM_MAX_READ_RETRIES       5u
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
static uint8_t  ym_read_retries;
static uint8_t  ym_can_count;
static uint32_t ym_deadline;
static uint32_t ym_gap_blocks;   /* diagnostic: blocks replaced by the marker below */
static uint32_t ym_bytes_sent;   /* running total, real + gap, vs. ym_size */
static bool     ym_hard_fault;   /* true once even f_lseek() past a bad block fails */

/* SOH/STX + block# + ~block# + up to 1024B payload + 2B CRC. CPU-only, never
 * touched by DMA (rb_write() memcpy's it into common_print_buffer), so no
 * SRAM3/alignment requirement like the DMA-visible buffers elsewhere. */
static uint8_t  ym_block_buf[3 + YM_BLOCK_SIZE + 2];
static uint32_t ym_block_len;

bool ymodem_active()
{
	return ym_state != YmState::Idle;
}

/* Blocks replaced by the gap marker in the transfer just finished (or in
 * progress). Reset at the start of the next transfer, not after this read. */
uint32_t ymodem_gap_blocks()
{
	return ym_gap_blocks;
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

enum class YmRead : uint8_t { Ok, Eof, Error };

/* Reads up to 1024B from the file into an STX data block, 0x1A-padding a
 * short final read. Ok/Eof are the only two outcomes that used to exist here
 * (collapsed into a single bool) -- Error is a distinct, third outcome: a
 * real f_read() failure (media error, or a distant seek on a file that
 * fell back to non-contiguous allocation, see sd_create_file()'s f_expand
 * comment) is NOT the same as having reached the end of the file, and must
 * never be treated as "transfer complete" by the caller. */
static YmRead ym_build_data_block(uint8_t blk)
{
	UINT     br      = 0;
	uint8_t *payload = ym_block_buf + 3;

	FRESULT res = f_read(&ym_fil, payload, YM_BLOCK_SIZE, &br);
	if (res != FR_OK) return YmRead::Error;
	if (br == 0)       return YmRead::Eof;

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
	return YmRead::Ok;
}

/* Unmistakable, human-readable filler for a block that f_read() could not
 * retrieve after YM_MAX_READ_RETRIES attempts (see the Error handling in
 * ymodem_poll()'s SendData case). Deliberately not zeros or any other value
 * that could pass for real telemetry, and deliberately not starting with
 * 'K'/'D' -- that's the SensorData_t record sync marker (see
 * find_resume_offset() in sd_task.cpp) and a gap block must never be
 * mistakable for a real, if garbled, record. */
static const char YM_GAP_MARKER[] =
	"*** GETLOG: SD READ FAILED HERE AFTER RETRIES -- DATA MISSING *** ";

static void ym_fill_gap_block(uint8_t blk)
{
	uint8_t *payload = ym_block_buf + 3;
	size_t   mlen    = sizeof(YM_GAP_MARKER) - 1;   /* exclude the NUL */

	for (size_t off = 0; off < YM_BLOCK_SIZE; off += mlen) {
		size_t n = (off + mlen <= YM_BLOCK_SIZE) ? mlen : (YM_BLOCK_SIZE - off);
		memcpy(payload + off, YM_GAP_MARKER, n);
	}

	ym_block_buf[0] = YM_STX;
	ym_block_buf[1] = blk;
	ym_block_buf[2] = (uint8_t)(0xFFu - blk);

	uint16_t crc = ym_crc16(payload, YM_BLOCK_SIZE);
	ym_block_buf[3 + YM_BLOCK_SIZE]     = (uint8_t)(crc >> 8);
	ym_block_buf[3 + YM_BLOCK_SIZE + 1] = (uint8_t)(crc & 0xFFu);
	ym_block_len = 3 + YM_BLOCK_SIZE + 2;
}

/* Builds and sends one gap block for the current block number, then arms
 * the normal ACK/NAK wait -- shared by every place that substitutes a gap
 * block, whether this is a single skipped block or part of a run of them
 * once ym_hard_fault is set. */
static void ym_send_gap_and_wait(uint8_t blk)
{
	ym_fill_gap_block(blk);
	ym_gap_blocks++;
	ym_bytes_sent += YM_BLOCK_SIZE;
	ym_send(ym_block_buf, ym_block_len);
	ym_retries = 0;
	ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
	ym_state = YmState::WaitDataResp;
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
	/* Deliberately no sd_card_state() check here: a file is open in the
	 * background almost continuously (one at boot, another after every
	 * mission ends), so SdState::Logging is the normal idle condition, not
	 * a sign anything is wrong. sd_get_last_log_name() below targets a
	 * different, already-closed file, so there's no conflict with whatever
	 * the SD task currently has open. */

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
		ym_size        = (uint32_t)f_size(&ym_fil);
		ym_block_num   = 1;
		ym_retries     = 0;
		ym_read_retries = 0;
		ym_can_count   = 0;
		ym_gap_blocks  = 0;
		ym_bytes_sent  = 0;
		ym_hard_fault  = false;

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

	case YmState::SendData: {
		/* Some receivers re-send 'C' right after ACKing block 0 to reconfirm
		 * CRC mode; discard anything pending here rather than misreading it
		 * as a NAK against the next data block. */
		(void)ym_rxq_pop(&c);

		if (ym_hard_fault) {
			/* A previous block's FAT chain link broke badly enough that
			 * even f_lseek() past it failed (see below) -- sequential
			 * FatFs access can never recover from a broken chain link, so
			 * there is no point calling f_read()/f_lseek() again for the
			 * rest of this file. Just account for the declared size
			 * (captured from f_size() back when the file was still fully
			 * readable) in gap blocks, no further FatFs calls at all.
			 * This is what guarantees the full declared length always
			 * gets delivered, however badly the tail of the file broke. */
			if (ym_bytes_sent >= ym_size) {
				ym_send_byte(YM_EOT);
				ym_retries = 0;
				ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
				ym_state = YmState::WaitEot1Resp;
				break;
			}
			ym_send_gap_and_wait(ym_block_num);
			break;
		}

		YmRead rr = ym_build_data_block(ym_block_num);

		if (rr == YmRead::Error) {
			/* A real f_read() failure -- e.g. a distant seek on a
			 * non-contiguously-allocated file timing out, or a genuine
			 * media error -- is NOT end of file. Treating it as EOF here
			 * would silently hand the receiver a truncated file it thinks
			 * is complete (which is what used to happen). Retry a few
			 * times first (poll-paced, so no busy-spin) in case it's
			 * transient -- f_read() leaves the file position at whatever
			 * it actually managed to read, so retrying the same call is
			 * correct, no seek needed for that part.
			 *
			 * If it's still failing after that many attempts, substitute
			 * a marker for this one block and try to skip the file
			 * position forward past it with f_lseek(), so the rest of the
			 * file -- which may still be fine -- keeps coming through. */
			if (++ym_read_retries <= YM_MAX_READ_RETRIES) break;

			FSIZE_t gap_pos = f_tell(&ym_fil);
			if (f_lseek(&ym_fil, gap_pos + YM_BLOCK_SIZE) != FR_OK) {
				/* Seeking forward has to walk the same FAT chain
				 * f_read() walks, so failing here means a broken chain
				 * link, not just one bad data sector -- no amount of
				 * further nudging will get past it. Switch to
				 * ym_hard_fault instead of aborting: gap-fill this block
				 * and every remaining one by byte count against ym_size,
				 * touching FatFs no further. */
				ym_hard_fault = true;
			}
			ym_read_retries = 0;
			ym_send_gap_and_wait(ym_block_num);
			break;
		}
		ym_read_retries = 0;

		if (rr == YmRead::Eof) {
			ym_send_byte(YM_EOT);
			ym_retries = 0;
			ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
			ym_state = YmState::WaitEot1Resp;
			break;
		}
		ym_bytes_sent += YM_BLOCK_SIZE;
		ym_send(ym_block_buf, ym_block_len);
		ym_retries = 0;
		ym_arm_timeout(YM_BLOCK_TIMEOUT_MS);
		ym_state = YmState::WaitDataResp;
		break;
	}

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
