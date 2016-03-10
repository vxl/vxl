#ifndef vcl_cstdlib_h_
#define vcl_cstdlib_h_

#include "vcl_compiler.h"
#include <cstdlib>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_abs std::abs
#define vcl_div std::div
#define vcl_labs std::labs
#define vcl_ldiv std::ldiv
#define vcl_srand std::srand
#define vcl_rand std::rand
#define vcl_atexit std::atexit
#define vcl_getenv std::getenv
#define vcl_system std::system
#define vcl_exit std::exit
#define vcl_abort std::abort
#define vcl_qsort std::qsort
#define vcl_calloc std::calloc
#define vcl_malloc std::malloc
#define vcl_free std::free
#define vcl_realloc std::realloc
#define vcl_atol std::atol
#define vcl_atof std::atof
#define vcl_atoi std::atoi
#define vcl_mblen std::mblen
#define vcl_mbstowcs std::mbstowcs
#define vcl_mbtowc std::mbtowc
#define vcl_strtod std::strtod
#define vcl_strtol std::strtol
#define vcl_strtoul std::strtoul
#define vcl_wctomb std::wctomb
#define vcl_wcstombs std::wcstombs

#endif // vcl_cstdlib_h_
