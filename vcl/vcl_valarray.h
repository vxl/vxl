#ifndef vcl_valarray_h_
#define vcl_valarray_h_

#include "vcl_compiler.h"
#include <valarray>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_cmath.h"
//vcl alias names to std names
#define vcl_valarray std::valarray
#define vcl_slice std::slice
#define vcl_slice_array std::slice_array
#define vcl_gslice std::gslice
#define vcl_gslice_array std::gslice_array
#define vcl_mask_array std::mask_array
#define vcl_indirect_array std::indirect_array

#endif // vcl_valarray_h_
