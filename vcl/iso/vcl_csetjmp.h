#ifndef vcl_iso_csetjmp_h_
#define vcl_iso_csetjmp_h_

#include <csetjmp>

// jmp_buf
#ifndef vcl_jmp_buf
#define vcl_jmp_buf std::jmp_buf
#endif
// longjmp
#ifndef vcl_longjmp
#define vcl_longjmp std::longjmp
#endif

#endif // vcl_iso_csetjmp_h_
