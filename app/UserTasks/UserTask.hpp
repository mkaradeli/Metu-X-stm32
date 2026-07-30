/*
 * UserTask.hpp
 *
 *  Created on: Jul 7, 2026
 *      Author: karadeli
 */

#ifndef USERTASKS_USERTASK_HPP_
#define USERTASKS_USERTASK_HPP_

#include "stdint.h"
#include "MissionControl.hpp"

void taskFunction(uint32_t time_ms);
void shutdownFunction(uint32_t time_ms);

extern uint32_t ops_duration_ms; // ms
extern uint32_t shutdown_duration_ms; // ms
extern uint32_t postShutdownWait_ms;
extern controller_modes actuator_mode_desired;

#endif /* USERTASKS_USERTASK_HPP_ */
