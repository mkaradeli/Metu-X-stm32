/*
 * profiler.cpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#include "Profiler.hpp"
#include "app_main.hpp"

Profiler::Profiler() {
	total_cycles = 0;
	elapsed_cycles = 0;  // cycles in 200MHz
	cpu_usage = 0; // %
	call_count = 0;
	mean_time = 0; // ms
	start_global = 0;
}
void Profiler::reset(){
	start_global = micros();
}

void Profiler::start() {
	start_call = micros();
}

void Profiler::end() {
	elapsed_cycles = micros() - start_call;
	total_cycles += elapsed_cycles;
	call_count += 1;
}
void Profiler::metrics(){
	cpu_usage = (float)(total_cycles * 100) / (micros() - start_global);
	mean_time =  (float)(total_cycles / call_count)/200;
	call_frequency = (200000000.0f*call_count) / (micros() - start_global);
}
