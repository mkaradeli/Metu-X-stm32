/*
 * ring_buffer.c
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */




#include "ring_buffer.h"
#include "stm32h7xx_hal.h"              /* for __DMB(); swap header if needed */

extern int __io_putchar(int ch);
__attribute__((section(".ramd3"), used))
volatile common_print_buffer_t common_print_buffer;

void rb_init(volatile common_print_buffer_t* cpb)
{
	cpb->head = 0;
	cpb->tail = 0;
	cpb->dropped = 0;
}

bool rb_push(volatile common_print_buffer_t* cpb, char c)
{
    uint32_t h    = cpb->head;
    uint32_t next = (h + 1u) & RB_MASK;

    if (next == cpb->tail) {                 /* full */
    	cpb->dropped++;
        return false;
    }
    cpb->buffer[h] = c;
    __DMB();                            /* byte visible before index update */
    cpb->head = next;
    return true;
}

size_t rb_push_n(volatile common_print_buffer_t* cpb, const char *p, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++) {
        if (!rb_push(cpb, p[i])) break;
    }
    return i;
}

int rb_pop(volatile common_print_buffer_t* cpb, char *c)
{
    uint32_t t = cpb->tail;
    if (t == cpb->head) return 0;            /* empty */
    *c = cpb->buffer[t];
    __DMB();
    cpb->tail = (t + 1u) & RB_MASK;
    return 1;
}

size_t rb_count(volatile common_print_buffer_t* cpb)
{

	return (size_t)((cpb->head - cpb->tail) & RB_MASK);
}

size_t rb_dropped(volatile common_print_buffer_t* cpb)
{
    return cpb->dropped;
}

void rb_flush(volatile common_print_buffer_t* cpb)
{
    char c;
    while (rb_pop(cpb, &c)) {
        __io_putchar(c);
    }
}
