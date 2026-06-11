
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

void   rb_init(void);
bool   rb_push(char c);                 /* returns false if full (drops newest) */
size_t rb_push_n(const char *p, size_t n); /* returns bytes actually pushed */
int    rb_pop(char *c);                 /* returns 1 if a byte was read, else 0 */
size_t rb_count(void);
size_t rb_dropped(void);                /* total bytes lost to overflow */
void   rb_flush(void);                  /* drains to __io_putchar */

extern volatile uint32_t head;
extern volatile uint32_t tail;
extern volatile uint32_t dropped;
extern char              buffer[RB_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* INC_RING_BUFFER_H_ */
