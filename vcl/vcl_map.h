#ifndef vcl_map_h_
#define vcl_map_h_

#include "vcl_compiler.h"
#include <map>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_functional.h"
#include "vcl_algorithm.h"
//vcl alias names to std names
#define vcl_map std::map
#define vcl_multimap std::multimap

#endif // vcl_map_h_
