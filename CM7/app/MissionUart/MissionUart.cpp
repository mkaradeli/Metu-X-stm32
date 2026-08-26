/*
 * MissionUart.cpp
 *
 *  Created on: Aug 16, 2026
 *      Author: karadeli
 *
 * Why polled circular DMA and not ReceiveToIdle
 * ---------------------------------------------
 * ReceiveToIdle in normal mode (what the lidar uses) has to be re-armed from
 * the callback, and bytes that land between the idle event and the re-arm are
 * gone. That is fine for a sensor that repeats itself 100 times a second. It
 * is not fine for a link that carries ABORT.
 *
 * So: the DMA runs circular and forever, nothing re-arms it, and the main
 * loop just compares the DMA write index against its own read index. No ISR
 * work, no lost bytes, no dependency on idle-line detection. The only failure
 * mode left is an overrun error killing the stream, which the poll notices
 * and restarts.
 *
 * Everything below runs in task context, so printf and snprintf are safe.
 */

#include "MissionUart.hpp"
#include "MissionControl.hpp"
#include <stdio.h>
#include <string.h>

#define CMD_RX_DMA_LEN   256u    /* circular DMA landing zone   */
#define CMD_LINE_MAX      96u    /* longest accepted command    */
#define CMD_REPLY_MAX    640u    /* LIST of ~8 missions fits    */

/* DMA cannot reach DTCM, so the landing zone lives in .sram3 (MPU region 1,
 * non-cacheable) like the ADC and IMU DMA buffers -- no cache maintenance
 * needed. 32-byte aligned so it never shares a cache line with a neighbour. */
__attribute__((section(".sram3"), aligned(32), used))
static uint8_t cmd_rx_dma[CMD_RX_DMA_LEN];

static UART_HandleTypeDef *cmd_huart = NULL;
static uint16_t cmd_rx_tail = 0;         /* our read index into cmd_rx_dma */

static char     cmd_line[CMD_LINE_MAX];
static uint16_t cmd_len      = 0;
static bool     cmd_too_long = false;

static uint32_t stat_restarts = 0;
static uint32_t stat_lines    = 0;

uint32_t mission_uart_restarts() { return stat_restarts; }
uint32_t mission_uart_lines()    { return stat_lines; }

/* ------------------------------------------------------------------ */

static void cmd_rx_start()
{
	if (cmd_huart == NULL) return;

	HAL_UART_AbortReceive(cmd_huart);
	__HAL_UART_CLEAR_OREFLAG(cmd_huart);
	__HAL_UART_CLEAR_FEFLAG(cmd_huart);
	__HAL_UART_CLEAR_NEFLAG(cmd_huart);

	cmd_rx_tail  = 0;
	cmd_len      = 0;
	cmd_too_long = false;

	memset(cmd_rx_dma, 0, sizeof(cmd_rx_dma));
//	SCB_CleanDCache_by_Addr((uint32_t *)cmd_rx_dma, sizeof(cmd_rx_dma));

	HAL_UART_Receive_DMA(cmd_huart, cmd_rx_dma, CMD_RX_DMA_LEN);
}

void mission_uart_init(UART_HandleTypeDef *huart)
{
	cmd_huart = huart;
	cmd_rx_start();
	printf("mission uart ready, type LIST\n\r");
}

void mission_uart_error(UART_HandleTypeDef *huart)
{
	if (huart == cmd_huart) {
		stat_restarts++;
		cmd_rx_start();
	}
}

/* ------------------------------------------------------------------ */

static void cmd_dispatch()
{
	if (cmd_too_long) {
		printf("cmd > %u chars, dropped\n\r", (unsigned)(CMD_LINE_MAX - 1));
		cmd_too_long = false;
		cmd_len = 0;
		return;
	}
	if (cmd_len == 0) return;          /* bare newline, ignore */

	cmd_line[cmd_len] = '\0';
	cmd_len = 0;
	stat_lines++;

	char reply[CMD_REPLY_MAX];
	reply[0] = '\0';

	missionControl.HandleCommand(cmd_line, reply, sizeof(reply));

	if (reply[0]) printf("%s", reply);
}

static void cmd_feed(char c)
{
	switch (c) {
	case '\r':
	case '\n':
		cmd_dispatch();
		break;

	case '\b':
	case 0x7F:                          /* backspace / DEL from a terminal */
		if (cmd_len) cmd_len--;
		break;

	case '\0':
		break;                          /* line noise on an idle line */

	default:
		if (c < ' ' || c > '~') break;  /* ignore control and 8-bit bytes */
		if (cmd_len + 1u < CMD_LINE_MAX) {
			cmd_line[cmd_len++] = c;
		} else {
			cmd_too_long = true;        /* keep eating until the newline */
		}
		break;
	}
}

void mission_uart_poll()
{
	if (cmd_huart == NULL || cmd_huart->hdmarx == NULL) return;

	/* Overrun, framing error or an aborted stream leaves RxState idle.
	 * Restart and drop whatever half-line was in flight. */
	if (cmd_huart->RxState != HAL_UART_STATE_BUSY_RX) {
		stat_restarts++;
		cmd_rx_start();
		return;
	}

	/* Invalidate before reading: the DMA wrote around the cache. Harmless
	 * if the MPU already marks this region non-cacheable. */
//	SCB_InvalidateDCache_by_Addr((uint32_t *)cmd_rx_dma, sizeof(cmd_rx_dma));

	uint16_t head = (uint16_t)(CMD_RX_DMA_LEN
	                           - __HAL_DMA_GET_COUNTER(cmd_huart->hdmarx));
	if (head >= CMD_RX_DMA_LEN) head = 0;

	while (cmd_rx_tail != head) {
		char c = (char)cmd_rx_dma[cmd_rx_tail];
		if (++cmd_rx_tail >= CMD_RX_DMA_LEN) cmd_rx_tail = 0;
		cmd_feed(c);
	}
}
