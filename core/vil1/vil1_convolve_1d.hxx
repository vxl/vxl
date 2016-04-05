// This is core/vil1/vil1_convolve_1d.hxx
#ifndef vil1_convolve_1d_hxx_
#define vil1_convolve_1d_hxx_
//:
// \file
// \author fsm

#include "vil1_convolve.h"

// Macros to get origin-relative values in signals.
#define value1d(sig, k)    ( (sig).array_[(sig).origin_ + (k)] )
#define value2d(sig, x, y) ( (sig).array_[(sig).originy_ + (y)][(sig).originx_ + (x)] )

// To make it easier to compare the x and y versions, they live
// in separate source files.
#define fsm_dont_croak
// Known problems : the convolvers may segfault if the kernel
//                  is larger than the input signal.
#include "vil1_convolve_1d_x.hxx"
#include "vil1_convolve_1d_y.hxx"

#undef VIL1_CONVOLVE_1D_INSTANTIATE
#define VIL1_CONVOLVE_1D_INSTANTIATE(I1, I2, AC, O) \
template \
void vil1_convolve_1d_x(vil1_convolve_signal_1d<const I1 > const &, \
                        vil1_convolve_signal_2d<const I2 > const &, \
                        AC *, \
                        vil1_convolve_signal_2d<O > const &, \
                        vil1_convolve_boundary_option, \
                        vil1_convolve_boundary_option); \
template \
void vil1_convolve_1d_y(vil1_convolve_signal_1d<const I1 > const &, \
                        vil1_convolve_signal_2d<const I2 > const &, \
                        AC *, \
                        vil1_convolve_signal_2d<O > const &, \
                        vil1_convolve_boundary_option, \
                        vil1_convolve_boundary_option)

#endif // vil1_convolve_1d_hxx_
