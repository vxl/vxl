// This is core/vgl/vgl_homg.cxx

#if !VXL_LEGACY_FUTURE_REMOVE
#  include "vgl_homg.h"
// template <> float vgl_homg<float>::infinity = 3.4028234663852886e+38f;
template <>
float vgl_homg<float>::infinitesimal_tol = 1e-12f;

// template <> double vgl_homg<double>::infinity = 1.7976931348623157e+308;
template <>
double vgl_homg<double>::infinitesimal_tol = 1e-12;

// template <> long double vgl_homg<long double>::infinity = 1.7976931348623157e+308;
template <>
long double vgl_homg<long double>::infinitesimal_tol = 1e-12;
#else
extern int vgl_homg_dummy_var; // avoid warning about empty symbol
int vgl_homg_dummy_var = 12345678;
#endif
