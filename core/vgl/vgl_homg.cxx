// This is core/vgl/vgl_homg.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vgl_homg.h"

VCL_DEFINE_SPECIALIZATION float vgl_homg<float>::infinity = 3.4028234663852886e+38f;
VCL_DEFINE_SPECIALIZATION float vgl_homg<float>::infinitesimal_tol = 1e-12f;

VCL_DEFINE_SPECIALIZATION double vgl_homg<double>::infinity = 1.7976931348623157e+308;
VCL_DEFINE_SPECIALIZATION double vgl_homg<double>::infinitesimal_tol = 1e-12;

VCL_DEFINE_SPECIALIZATION long double vgl_homg<long double>::infinity = 1.7976931348623157e+308;
VCL_DEFINE_SPECIALIZATION long double vgl_homg<long double>::infinitesimal_tol = 1e-12;
