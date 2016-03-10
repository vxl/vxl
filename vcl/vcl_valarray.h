#ifndef vcl_valarray_h_
#define vcl_valarray_h_

#include "vcl_compiler.h"
#include <valarray>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_valarray std::valarray
#define vcl_slice std::slice
#define vcl_slice_array std::slice_array
#define vcl_gslice std::gslice
#define vcl_gslice_array std::gslice_array
#define vcl_mask_array std::mask_array
#define vcl_indirect_array std::indirect_array
#define vcl_abs std::abs
#define vcl_acos std::acos
#define vcl_asin std::asin
#define vcl_atan std::atan
#define vcl_atan2 std::atan2
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

#endif // vcl_valarray_h_
