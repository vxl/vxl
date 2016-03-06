#ifndef vcl_complex_h_
#define vcl_complex_h_

#include "vcl_compiler.h"
#include <complex>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_complex std::complex
#define vcl_real std::real
#define vcl_imag std::imag
#define vcl_abs std::abs
#define vcl_arg std::arg
#define vcl_norm std::norm
#define vcl_conj std::conj
#define vcl_polar std::polar
#define vcl_cos std::cos
#define vcl_cosh std::cosh
#define vcl_exp std::exp
#define vcl_log std::log
#define vcl_log10 std::log10
#define vcl_pow std::pow
#define vcl_sin std::sin
#define vcl_sinh std::sinh
#define vcl_sqrt std::sqrt
#define vcl_tan std::tan
#define vcl_tanh std::tanh

#endif // vcl_complex_h_
