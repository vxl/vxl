#ifndef vcl_iso_complex_h_
#define vcl_iso_complex_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <complex>

#define vcl_complex std::complex

#ifndef vcl_abs
#define vcl_abs   std::abs
#endif
#define vcl_conj  std::conj
#define vcl_norm  std::norm
#ifndef vcl_sqrt
#define vcl_sqrt  std::sqrt
#endif
#define vcl_arg   std::arg
#define vcl_polar std::polar
#define vcl_real  std::real
#define vcl_imag  std::imag

#endif
