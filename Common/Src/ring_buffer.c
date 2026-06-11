/*
 * ring_buffer.c
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */




#include "ring_buffer.h"
#include "stm32h7xx_hal.h"              /* for __DMB(); swap header if needed */

extern int __io_putchar(int ch);

/* Lamport SPSC: producer owns head, consumer owns tail.
 * Both 32-bit aligned -> single-word loads/stores are atomic on Cortex-M. */
__attribute__((section(".shared_memory"), used))
 volatile uint32_t head;
__attribute__((section(".shared_memory"), used))
volatile uint32_t tail;
__attribute__((section(".shared_memory"), used))
volatile uint32_t dropped;
__attribute__((section(".shared_memory"), used))
char              buffer[RB_SIZE];

void rb_init(void)
{
    head = 0;
    tail = 0;
    dropped = 0;
}

bool rb_push(char c)
{
    uint32_t h    = head;
    uint32_t next = (h + 1u) & RB_MASK;

    if (next == tail) {                 /* full */
        dropped++;
        return false;
    }
    buffer[h] = c;
    __DMB();                            /* byte visible before index update */
    head = next;
    return true;
}

size_t rb_push_n(const char *p, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++) {
        if (!rb_push(p[i])) break;
    }
    return i;
}

int rb_pop(char *c)
{
    uint32_t t = tail;
    if (t == head) return 0;            /* empty */
    *c = buffer[t];
    __DMB();
    tail = (t + 1u) & RB_MASK;
    return 1;
}

size_t rb_count(void)
{

	return (size_t)((head - tail) & RB_MASK);
}

size_t rb_dropped(void)
{
    return dropped;
}

void rb_flush(void)
{
    char c;
    while (rb_pop(&c)) {
        __io_putchar(c);
    }
}
