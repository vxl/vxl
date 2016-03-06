#ifndef vcl_utility_h_
#define vcl_utility_h_

#include "vcl_compiler.h"
#include <utility>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_pair std::pair
#define vcl_make_pair std::make_pair

#endif // vcl_utility_h_
