#ifndef vcl_cstddef_h_
#define vcl_cstddef_h_

#include "vcl_compiler.h"
#include <cstddef>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_ptrdiff_t std::ptrdiff_t
#define vcl_size_t std::size_t

#endif // vcl_cstddef_h_
