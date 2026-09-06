/*
 * MissionUart.hpp
 *
 *  Created on: Aug 16, 2026
 *      Author: karadeli
 *
 *  Line-based command receiver for the ground link. Feeds complete lines to
 *  MissionControl::HandleCommand() and prints the reply through printf, so
 *  it comes back out on the same UART as the rest of the console output.
 *
 *  Not a file the mission team edits.
 */

#ifndef MISSIONUART_HPP_
#define MISSIONUART_HPP_

#include "stm32h7xx_hal.h"

/* The DMA stream for this UART's RX must be set to CIRCULAR mode in CubeMX.
 * Call once, after MX_USARTx_UART_Init() and missionControl.Init(). */
void mission_uart_init(UART_HandleTypeDef *huart);

/* Call from the main loop. Cheap: one DMA counter read when idle. */
void mission_uart_poll();

/* Call from HAL_UART_ErrorCallback() for this UART. */
void mission_uart_error(UART_HandleTypeDef *huart);

/* Raw takeover: while a sink is registered, mission_uart_poll() hands every
 * drained byte to it instead of feeding the ASCII line parser. The RX DMA
 * itself is untouched -- only how drained bytes are interpreted changes.
 * `sink` runs in the same context as mission_uart_poll() (the TIM7 ISR):
 * keep it tiny, no FatFs, no blocking. Used by YModem for the GETLOG
 * transfer so it can steal this UART's RX without duplicating the DMA
 * drain logic above. */
void mission_uart_enter_raw(void (*sink)(uint8_t c));

/* Clears the raw sink and resets the ASCII console (calls cmd_rx_start()) so
 * leftover protocol bytes can't leak into the next typed command. */
void mission_uart_exit_raw();

/* Diagnostics for the 1 Hz printf block. */
uint32_t mission_uart_restarts();
uint32_t mission_uart_lines();

#endif /* MISSIONUART_HPP_ */
