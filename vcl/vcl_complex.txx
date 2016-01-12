#ifndef vcl_complex_txx_
#define vcl_complex_txx_
// -*- c++ -*-

#include "vcl_complex.h"

#include "iso/vcl_complex.txx"

#if defined(VCL_GCC_31) && defined(sun)
# include "gcc/vcl_cmath.h" // for sqrtf(), sinf(), cosf(), sinl(), cosl()
#endif // sun

#endif
