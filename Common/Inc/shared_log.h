/*
 * shared_log.h
 *
 *  Created on: May 24, 2026
 *      Author: karadeli
 */

#ifndef INC_SHARED_LOG_H_
#define INC_SHARED_LOG_H_

//#include <stdint.h>
//#include <stdbool.h>
////#include ""
//
//
//#define LOG_BUF_SIZE  4096u             // must be power of 2
//#define LOG_BUF_MASK  (LOG_BUF_SIZE - 1u)
//
//typedef struct {
//    volatile uint32_t head;             // producer writes (M7), consumer reads (M4)
//    volatile uint32_t tail;             // consumer writes (M4), producer reads (M7)
//    volatile uint32_t dropped;          // bytes lost when buffer was full
//    uint8_t  data[LOG_BUF_SIZE];
//} log_ring_t;

//extern log_ring_t shared_log;

//uint32_t  log_ring_push(const uint8_t *src, uint32_t len);  // call from M7
//uint32_t  log_ring_pop (uint8_t *dst, uint32_t max);        // call from M4
//
//static inline uint32_t log_ring_used(void)  { return shared_log.head - shared_log.tail; }
//static inline bool     log_ring_empty(void) { return shared_log.head == shared_log.tail; }



#endif /* INC_SHARED_LOG_H_ */
