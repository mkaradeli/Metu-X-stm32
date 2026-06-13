
/*
 * ring_buffer.h
 *
 *  Created on: May 25, 2026
 *      Author: karadeli
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define RB_SIZE  4096u                  /* must be power of 2 */
#define RB_MASK  (RB_SIZE - 1u)

typedef struct {
	volatile uint32_t head;
	volatile uint32_t tail;
	volatile uint32_t dropped;
	char buffer[RB_SIZE];
} common_print_buffer_t;

void   rb_init(volatile common_print_buffer_t* cpb);
bool   rb_push(volatile common_print_buffer_t* cpb, char c);                 /* returns false if full (drops newest) */
size_t rb_push_n(volatile common_print_buffer_t* cpb, const char *p, size_t n); /* returns bytes actually pushed */
int    rb_pop(volatile common_print_buffer_t* cpb, char *c);                 /* returns 1 if a byte was read, else 0 */
size_t rb_count(volatile common_print_buffer_t* cpb);
size_t rb_dropped(volatile common_print_buffer_t* cpb);                /* total bytes lost to overflow */
void   rb_flush(volatile common_print_buffer_t* cpb);                  /* drains to __io_putchar */

//extern volatile uint32_t head;
//extern volatile uint32_t tail;
//extern volatile uint32_t dropped;
//extern char              buffer[RB_SIZE];



extern volatile common_print_buffer_t common_print_buffer;

#ifdef __cplusplus
}
#endif

#endif /* INC_RING_BUFFER_H_ */
