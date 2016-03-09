#ifndef vcl_complex_h_
#define vcl_complex_h_

#include "vcl_compiler.h"
#include "vcl_cmath.h"

// File: vcl_complex.h
//
// All supported compiler currently provide rational complex
// number support.  No specializations needed.

#include <complex>

#define vcl_complex std::complex
#define vcl_real std::real
#define vcl_imag std::imag
#define vcl_arg std::arg
#define vcl_norm std::norm
#define vcl_conj std::conj
#define vcl_polar std::polar

#endif // vcl_complex_h_
