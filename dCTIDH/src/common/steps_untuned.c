#include "steps.h"

int steps_guess(int64_t *bs,int64_t *gs,int64_t l)
{
  if (l == 587) {
    *bs = 16;
    *gs = 9;
    return 1;
  }
  return 0;
}
