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

/* Diagnostics for the 1 Hz printf block. */
uint32_t mission_uart_restarts();
uint32_t mission_uart_lines();

#endif /* MISSIONUART_HPP_ */
