// This is vxl/vgl/vgl_homg.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vgl_homg.h"

float vgl_homg<float>::infinity = 3.4028234663852886e+38f;
float vgl_homg<float>::infinitesimal_tol = 1e-12f;

double vgl_homg<double>::infinity = 1.7976931348623157e+308;
double vgl_homg<double>::infinitesimal_tol = 1e-12;

long double vgl_homg<long double>::infinity = 1.7976931348623157e+308;
long double vgl_homg<long double>::infinitesimal_tol = 1e-12;
