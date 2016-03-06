#ifndef vcl_limits_h_
#define vcl_limits_h_

#include "vcl_compiler.h"
#include <limits>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_numeric_limits std::numeric_limits
#define vcl_float_round_style std::float_round_style
#define vcl_float_denorm_style std::float_denorm_style
#define vcl_round_toward_zero std::round_toward_zero
#define vcl_round_toward_neg_infinity std::round_toward_neg_infinity

#endif // vcl_limits_h_
