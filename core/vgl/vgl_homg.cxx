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

#ifdef VCL_BORLAND_56
// Not sure why this is needed for Borland 5.6.  Bug?
// Alternatively we could define separate class template
// specializations for every supported vgl_homg type and then just
// have an empty function here.
void vgl_homg_linker_hack()
{
  (void)vgl_homg<float>::infinity;
  (void)vgl_homg<float>::infinitesimal_tol;
  (void)vgl_homg<double>::infinity;
  (void)vgl_homg<double>::infinitesimal_tol;
  (void)vgl_homg<long double>::infinity;
  (void)vgl_homg<long double>::infinitesimal_tol;
}
#endif
