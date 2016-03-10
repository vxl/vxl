#ifndef vcl_stack_h_
#define vcl_stack_h_

#include "vcl_compiler.h"
#include <stack>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_stack std::stack

#endif // vcl_stack_h_
