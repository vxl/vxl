#ifdef __GNUG__
#pragma implementation
#endif

#include "Homg.h"
#include <vcl_cmath.h>
#ifdef VCL_SUNPRO_CC_50
# include <math.h> // no HUGE_VAL in <cmath>
#endif

double Homg::infinity = HUGE_VAL;
double Homg::infinitesimal_tol = 1e-12;
