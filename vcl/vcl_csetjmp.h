#ifndef vcl_csetjmp_h_
#define vcl_csetjmp_h_

#include "vcl_compiler.h"
#include <csetjmp>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_jmp_buf std::jmp_buf
#define vcl_longjmp std::longjmp

#endif // vcl_csetjmp_h_
