#include <stdint.h>
#include <stddef.h>

#ifndef HAL_H
#define HAL_H

enum clock_mode {
    CLOCK_FAST,
    CLOCK_BENCHMARK
};

void hal_setup(const enum clock_mode clock);
uint64_t hal_get_time(void);

#endif
