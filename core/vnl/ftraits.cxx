/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif

// This is a traits experiment.
// I want to know which compilers it works on.
// If this breaks, just #ifdef it out for your compiler.

#include "ftraits.h"

vcl_complex<float> f(float x, float y);

#include <vnl/vnl_complex.h>

vcl_float_complex f(float x, float y) {
  return vcl_float_complex(x, y);
}

//--------------------------------------------------------------------------------
