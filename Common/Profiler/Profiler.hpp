/*
 * profiler.hpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#ifndef PROFILER_PROFILER_HPP_
#define PROFILER_PROFILER_HPP_

#include <stdint.h>
#include "stm32h7xx_hal.h"

class Profiler{
public:
	Profiler();
	float cpu_usage;
	float mean_time;
	float call_frequency;
	void start();
	void end();
	void metrics();
	void reset();
	uint32_t get_start_click() {
		return start_call;
	}
private:
	void cont();
	void pause();
	bool is_running;
	bool is_paused;
	Profiler *parent_profiler;
	uint64_t total_cycles;
	uint64_t elapsed_cycles;
	uint64_t start_call;
	uint64_t start_global;
	uint64_t call_count;
	uint32_t double_start_count;
	uint32_t double_end_count;
};



#endif /* PROFILER_PROFILER_HPP_ */
