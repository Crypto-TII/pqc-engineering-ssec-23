#ifndef BENCHMARKS_UTILS_C
#define BENCHMARKS_UTILS_C

#include <time.h>
#include "cycle.h"
#include "common/hal.h"
#include <utilities.h>

static inline void hal_init_perfcounters(int do_reset, int enable_divider);
static unsigned long long get_nanos(void);
static unsigned long long get_timestamp(void);


/// =========================== LIBRARY ===========================

static unsigned long long get_timestamp(void) {

    unsigned long long timestamp = 0;
#if (defined(CYCLES) && (defined(AARCH64) || defined(ARMV7l)))
    timestamp = (unsigned long long) hal_get_time();
#elif (defined(CYCLES) && defined(x8664))
    timestamp = (unsigned long long) getticks();
#elif defined(TIME)
    timestamp = get_nanos();
#endif
    return timestamp;
}


static unsigned long long get_nanos(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (unsigned long long)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}

void printStatisticsFile(const char* file_name, ticks* two_isogeny_walk, ticks* three_isogeny_walk) {

    FILE* statsFile;
    //char fileNameBuffer[100];
    statsFile = fopen(file_name, "w+");

    fprintf(statsFile, "No.,2-isogeny-walk,3-isogeny-walk\n");

    for (int i = 0; i < NUMBER_OF_BENCHMARK_EXPERIMENTS; i++)
    {
        fprintf(statsFile, "%d,%llu,%llu\n", i, (unsigned long long)two_isogeny_walk[i], (unsigned long long)three_isogeny_walk[i]);
    }

    fclose(statsFile);

}

static inline void hal_init_perfcounters(int do_reset, int enable_divider)
{

#if defined(CYCLES)

    #if defined(AARCH64)
    printf("%d, %d\n", do_reset, enable_divider);
    #elif defined(ARMV7l)

        // in general enable all counters (including cycle counter)
        int value = 1;

    // perform reset:
    if (do_reset)
    {
        value |= 2;     // reset all counters to zero.
        value |= 4;     // reset cycle counter to zero.
    }

    if (enable_divider)
        value |= 8;     // enable "by 64" divider for CCNT.

    value |= 16;

    // program the performance-counter control-register:
    asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));

    // enable all counters:
    asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));

    // clear overflows:
    asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));

#endif

#endif
}


#endif