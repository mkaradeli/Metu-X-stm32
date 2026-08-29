/*
 * profiler.cpp
 *
 *  Created on: Jan 17, 2026
 *      Author: karadeli
 */

#include "Profiler.hpp"
#include "app_main.hpp"

static Profiler* g_active = nullptr;

Profiler::Profiler(const char *name_val) : name(name_val) {
	total_cycles = 0;
	elapsed_cycles = 0;  // cycles in 1MHz
	cpu_usage = 0; // %
	call_count = 0;
	mean_time = 0; // ms
	start_global = 0;
}
void Profiler::reset(){
//	uint64_t now = micros();
	total_cycles = 0;
	start_global = micros();
	call_count = 0;
}

void Profiler::start() {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
    if (is_running) { double_start_count++; __set_PRIMASK(primask); return; }
	parent_profiler = g_active;
	if (parent_profiler)
		parent_profiler->pause();
	g_active = this;
	is_running = true;
	is_paused = false;
	start_call = micros();
	__set_PRIMASK(primask);
	if (start_global == 0){
		start_global = start_call;
	}
}

void Profiler::pause() {
	if (!is_running || is_paused) return;
	elapsed_cycles = micros() - start_call;
	total_cycles += elapsed_cycles;
	is_paused = true;
}
void Profiler::cont() {
	if (!is_running || !is_paused) return;
	start_call = micros();
	is_paused = false;
}

void Profiler::end() {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
    if (!is_running) { double_end_count++; __set_PRIMASK(primask); return; }
	if (!is_paused) {
		elapsed_cycles  = micros() - start_call;
		total_cycles += elapsed_cycles;
	}
	call_count += 1;
	is_running = false;
	g_active = parent_profiler;
	if (parent_profiler)
		parent_profiler->cont();
	__set_PRIMASK(primask);
//	parent_profiler = nullptr;

}
void Profiler::metrics(){
    uint64_t elapsed = micros() - start_global;
    if (elapsed == 0) { cpu_usage = 0; call_frequency = 0; mean_time = 0; return; }

	cpu_usage = (float)(total_cycles * 100) / (float)elapsed;
	call_frequency = (1e6f*(float)call_count) / (float)elapsed;
	mean_time =  call_count? (float)total_cycles / ((float)call_count * 1.0f): 0.0f;
}
