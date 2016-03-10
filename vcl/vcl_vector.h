#ifndef vcl_vector_h_
#define vcl_vector_h_

#include "vcl_compiler.h"
#include <vector>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_vector std::vector
#define vcl_swap std::swap

#endif // vcl_vector_h_
