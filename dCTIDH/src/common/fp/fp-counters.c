#if GLOBAL_COUNTERS != 0

#include <stdint.h>
#include "fp-counters.h"

__thread uint64_t fpadd = 0; // counter of fp-additions
__thread uint64_t fpsqr = 0; // counter of fp-squarings
__thread uint64_t fpmul = 0;

#endif
