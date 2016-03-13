#ifndef vcl_complex_h_
#define vcl_complex_h_

#include "vcl_compiler.h"
#include <complex>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_cmath.h"
//vcl alias names to std names
#define vcl_complex std::complex
#define vcl_real std::real
#define vcl_imag std::imag
#define vcl_arg std::arg
#define vcl_norm std::norm
#define vcl_conj std::conj
#define vcl_polar std::polar

#endif // vcl_complex_h_
