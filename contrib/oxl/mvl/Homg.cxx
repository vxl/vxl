// This is oxl/mvl/Homg.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "Homg.h"
#include <vcl_cmath.h>
#ifdef VCL_SUNPRO_CC_50
# include <math.h> // dont_vxl_filter: no HUGE_VAL in <cmath>
#endif

double Homg::infinity = HUGE_VAL;
double Homg::infinitesimal_tol = 1e-12;
