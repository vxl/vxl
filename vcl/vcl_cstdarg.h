#ifndef vcl_cstdarg_h_
#define vcl_cstdarg_h_

#include "vcl_compiler.h"
#include <cstdarg>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_va_list std::va_list

#endif // vcl_cstdarg_h_
