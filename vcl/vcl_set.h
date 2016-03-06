#ifndef vcl_set_h_
#define vcl_set_h_

#include "vcl_compiler.h"
#include <set>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_set std::set
#define vcl_multiset std::multiset
#define vcl_swap std::swap

#endif // vcl_set_h_
