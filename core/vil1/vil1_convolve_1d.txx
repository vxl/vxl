// This is vxl/vil/vil_convolve_1d.txx
#ifndef vil_convolve_1d_txx_
#define vil_convolve_1d_txx_
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil_convolve.h"

// Macros to get origin-relative values in signals.
#define value1d(sig, k)    ( (sig).array_[(sig).origin_ + (k)] )
#define value2d(sig, x, y) ( (sig).array_[(sig).originy_ + (y)][(sig).originx_ + (x)] )

// To make it easier to compare the x and y versions, they live
// in separate source files.
#define fsm_dont_croak
// Known problems : the convolvers may segfault if the kernel
//                  is larger than the input signal.
#include "vil_convolve_1d_x.txx"
#include "vil_convolve_1d_y.txx"

#undef VIL_CONVOLVE_1D_INSTANTIATE
#define VIL_CONVOLVE_1D_INSTANTIATE(I1, I2, AC, O) \
template \
void vil_convolve_1d_x(vil_convolve_signal_1d<const I1 > const &, \
                       vil_convolve_signal_2d<const I2 > const &, \
                       AC *, \
                       vil_convolve_signal_2d<O > const &, \
                       vil_convolve_boundary_option, \
                       vil_convolve_boundary_option); \
template \
void vil_convolve_1d_y(vil_convolve_signal_1d<const I1 > const &, \
                       vil_convolve_signal_2d<const I2 > const &, \
                       AC *, \
                       vil_convolve_signal_2d<O > const &, \
                       vil_convolve_boundary_option, \
                       vil_convolve_boundary_option)

#endif // vil_convolve_1d_txx_
