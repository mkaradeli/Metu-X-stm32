/*
 * task_timer_t.h
 *
 *  Created on: Jun 13, 2026
 *      Author: karadeli
 */
#include <stdbool.h>
#include <main.h>

#ifndef INC_TASK_TIMER_H_
#define INC_TASK_TIMER_H_

typedef struct {
	uint32_t period;
	uint32_t last_trigger;
} task_timer_t;

bool task_ready(task_timer_t* task_timer);


#endif /* INC_TASK_TIMER_H_ */
