#ifdef __GNUG__
#pragma implementation
#endif

#include "vgl_homg.h"
#include <vcl_cmath.h>
#ifdef VCL_SUNPRO_CC_50
# include <math.h> // dont_vxl_filter: no HUGE_VAL in <cmath>
#endif

double vgl_homg<double>::infinity = HUGE_VAL;
double vgl_homg<double>::infinitesimal_tol = 1e-12;
