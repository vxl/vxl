#ifndef vcl_set_h_
#define vcl_set_h_

#include "vcl_compiler.h"
#include <set>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_algorithm.h"
//vcl alias names to std names
#define vcl_set std::set
#define vcl_multiset std::multiset

#endif // vcl_set_h_
