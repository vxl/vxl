#ifndef vcl_deque_h_
#define vcl_deque_h_

#include "vcl_compiler.h"
#include <deque>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_algorithm.h"
//vcl alias names to std names
#define vcl_deque std::deque

#endif // vcl_deque_h_
