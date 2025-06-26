#ifndef STEPS_H
#define STEPS_H

#include "../common/namespace.h"
#include <inttypes.h>

#define steps COMMON(steps)
#define steps_override COMMON(steps_override)
#define steps_guess COMMON(steps_guess)

/* assumes l >= 3, l odd */
/* guarantees (b,g) = (0,0) _or_ the following: */
/* b > 0; b is even; g > 0; 4*b*g <= l-1 */
/* tries to choose (b,g) sensibly */
void steps(int64_t *bs,int64_t *gs,int64_t l);

/* internal API for tuning to see bs,gs effects: */
void steps_override(int64_t bs,int64_t gs);

/* internal API for tuning to select bs,gs: */
int steps_guess(int64_t *bs,int64_t *gs,int64_t l);

#endif
