/*
 * task_timer.c
 *
 *  Created on: Jun 13, 2026
 *      Author: karadeli
 */

#include "task_timer.h"


//typedef struct {
//	uint32_t period;
//	uint32_t last_trigger;
//} task_timer_t;

bool task_ready(task_timer_t* task_timer) {
	if (task_timer->period >= uwTick - task_timer->last_trigger){
		task_timer->last_trigger += task_timer->period;
		return true;
	}
	return false;
}
