#ifndef vcl_numeric_h_
#define vcl_numeric_h_

#include "vcl_compiler.h"
#include <numeric>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_accumulate std::accumulate
#define vcl_inner_product std::inner_product
#define vcl_partial_sum std::partial_sum
#define vcl_adjacent_difference std::adjacent_difference

#endif // vcl_numeric_h_
