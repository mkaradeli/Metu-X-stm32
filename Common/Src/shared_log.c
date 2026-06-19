/*
// shared_log.c
#include "shared_log.h"
//#include "cmsis_compiler.h"   // __DMB, __DSB
#include <string.h>

// Placed by linker in SRAM4. Don't zero-init from either startup;
// the boot core initializes it once before releasing the second core.
__attribute__((section(".shared_memory"), used))
log_ring_t shared_log;

uint32_t log_ring_push(const uint8_t *src, uint32_t len)
{
    uint32_t head = shared_log.head;
    uint32_t tail = shared_log.tail;        // snapshot the other side
    uint32_t freeb = LOG_BUF_SIZE - (head - tail);

    if (len > freeb) {
        shared_log.dropped += (len - freeb);
        len = freeb;
    }
    if (len == 0) return 0;

    uint32_t idx   = head & LOG_BUF_MASK;
    uint32_t first = LOG_BUF_SIZE - idx;
    if (first > len) first = len;

    memcpy(&shared_log.data[idx], src, first);
    if (len > first)
        memcpy(&shared_log.data[0], src + first, len - first);

//    __DMB();                                 // data visible before head advance
    shared_log.head = head + len;
//    __DSB();
    return len;
}

uint32_t log_ring_pop(uint8_t *dst, uint32_t max)
{
    uint32_t head = shared_log.head;         // snapshot the other side
    uint32_t tail = shared_log.tail;
//    __DMB();                                 // observe head before reading data

    uint32_t avail = head - tail;
    if (avail == 0) return 0;
    if (max > avail) max = avail;

    uint32_t idx   = tail & LOG_BUF_MASK;
    uint32_t first = LOG_BUF_SIZE - idx;
    if (first > max) first = max;

    memcpy(dst, &shared_log.data[idx], first);
    if (max > first)
        memcpy(dst + first, &shared_log.data[0], max - first);

//    __DMB();
    shared_log.tail = tail + max;
    return max;
}
*/
