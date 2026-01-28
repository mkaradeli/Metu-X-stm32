/*
 * profiler.cpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#include "profiler.hpp"

Profiler::Profiler() {
	total_time = 0;
	last_delta = 0;
	cpu_usage = 0;
	call_count = 0;
	mean_time = 0;
	start_global = micros();

}


void Profiler::start() {
	start_call = micros();
}

void Profiler::end() {
	last_delta = micros() - start_call;
	total_time += last_delta;
	cpu_usage = total_time / (micros() - start_global);
	call_count += 1;
	mean_time =  total_time / call_count;

}
