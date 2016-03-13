#ifndef vcl_csetjmp_h_
#define vcl_csetjmp_h_

#include "vcl_compiler.h"
#include <csetjmp>
//vcl alias names to std names
#define vcl_jmp_buf std::jmp_buf
#define vcl_longjmp std::longjmp

#endif // vcl_csetjmp_h_
