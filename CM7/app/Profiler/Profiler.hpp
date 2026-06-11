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
	uint64_t total_cycles;
	uint32_t elapsed_cycles;
	uint32_t start_call;
	uint32_t start_global;
	uint32_t call_count;
};



#endif /* PROFILER_PROFILER_HPP_ */
