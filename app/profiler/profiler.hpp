/*
 * profiler.hpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#ifndef PROFILER_PROFILER_HPP_
#define PROFILER_PROFILER_HPP_

#include <stdint.h>
#include "tim.h"

class Profiler{
public:
	Profiler();
	float cpu_usage;
	float mean_time;
	float call_frequency;
	void start();
	void end();
private:
	uint32_t total_time;
	uint32_t last_delta;
	uint32_t start_call;
	uint32_t start_global;
	uint32_t call_count;
};



#endif /* PROFILER_PROFILER_HPP_ */
