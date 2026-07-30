/*
 * MissionControl.cpp
 *
 *  Created on: Jul 3, 2026
 *      Author: karadeli
 */

#include "MissionControl.hpp"
//#include "shared_memory.h"
#include "stm32h7xx_hal.h"

	MissionControl::MissionControl(volatile bool* log_ready,
			volatile bool* log_recording) {
		this->system_mode = system_modes::IDLE;
		this->system_mode_desired = system_modes::IDLE;
		this->actuator_mode = controller_modes::DISABLE;
		this->actuator_mode_desired = controller_modes::DISABLE;
		controller_mode = this->actuator_mode;
		this->log_ready = log_ready;
		this->log_recording = log_recording;
		this->ops_duration_ms = 0U;
		this->shutdown_duration_ms = 0U;
	}
	void MissionControl::Init(Function taskFunction, Function shutdownFunction) {
		this->taskFunction = taskFunction;
		this->shutdownFunction = shutdownFunction;

	}
	void MissionControl::Iter() {
		switch (system_mode) {
		case system_modes::IDLE:
		case system_modes::FAULT:
			break;
		case system_modes::TESTFIRE:
		case system_modes::HOVER:
		case system_modes::DROP:
			ops_time_counter_ms = uwTick - ops_start_ms;
			if (ops_time_counter_ms < ops_duration_ms)
				taskFunction(ops_time_counter_ms);
			else {
				system_mode = system_modes::SHUTDOWN;
				actuator_mode = controller_modes::CURRENT;
				controller_mode = this->actuator_mode;

				shutdown_start_ms = uwTick;
				shutdownFunction(0);

			}
			break;
		case system_modes::SHUTDOWN:
			ops_time_counter_ms = uwTick - ops_start_ms;
			shutdown_time_counter_ms = uwTick - shutdown_start_ms;

			if (shutdown_time_counter_ms < shutdown_duration_ms) {
				shutdownFunction(shutdown_time_counter_ms);
			}
			else if(shutdown_time_counter_ms< shutdown_duration_ms+postShutdownWait_ms) {
				system_mode = system_modes::IDLE;
				actuator_mode = controller_modes::DISABLE;
				controller_mode = this->actuator_mode;
			}
			else {
				*log_recording = false;
				running = false;
			}
			break;
		default:
			system_mode = system_modes::FAULT;
			break;

		}
	}
	bool MissionControl::Start() {
		bool returnValue = false;
	    if (system_mode != system_modes::IDLE) return returnValue;

		if (!*log_ready) {
			return returnValue;
		}
		if (shutdownFunction == nullptr or taskFunction == nullptr) return returnValue;

		switch (system_mode_desired) {
		case system_modes::TESTFIRE:
		case system_modes::HOVER:
		case system_modes::DROP:
			*log_recording = true;
			system_mode = system_mode_desired;
			actuator_mode = actuator_mode_desired;
			controller_mode = this->actuator_mode;

			ops_time_counter_ms = 0;
			ops_start_ms = uwTick;
			returnValue = true;
			running = true;

			break;
		default:
			system_mode = system_modes::IDLE;
			actuator_mode = controller_modes::DISABLE;
			controller_mode = this->actuator_mode;
			break;

		}
		return returnValue;

	}
	void MissionControl::End() {
		system_mode = system_modes::IDLE;
		actuator_mode = controller_modes::DISABLE;
		controller_mode = this->actuator_mode;
		*log_recording = false;
		running = false;

	}

	void MissionControl::Toggle() {
		if (!running)
			this->Start();
		else
			this->End();

	}
