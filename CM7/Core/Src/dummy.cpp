/*
 * dummy.cpp
 *
 *  Created on: May 13, 2026
 *      Author: karadeli
 */




#include "dummy.hpp"

namespace {

class Counter {
public:
    constexpr Counter(uint32_t start) : value_(start) {}

    uint32_t next() {
        value_ += step_;
        return value_;
    }

    void reset() { value_ = 0; }

private:
    static constexpr uint32_t step_ = 1;
    uint32_t value_;
};

// Static instance — its constructor runs before main() via __libc_init_array.
// If this value isn't 42 when dummy_init() is first called, your C++ runtime
// init is broken (linker is probably still using gcc instead of g++).
Counter g_counter{42};

}  // anonymous namespace

void dummy_init(void) {
    g_counter.reset();
}

uint32_t dummy_tick(void) {
    return g_counter.next();
}
