#include "f2c.h"
#include <math.h>

/* extern "C" */
double pow_dd(const double *x, const double *y) {
  return pow(*x, *y);
}
