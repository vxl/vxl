// This is vcl/vcl_cstdlib.h
#ifndef vcl_cstdlib_h_
#define vcl_cstdlib_h_
//:
// \file
// Early compilers required work arounds for
// these functions that are not longer needed
// for the current set of supported compilers.
// greatly simplifying the aliasing of fuctions
// that are never changed.
// NB: size_t is declared in <cstddef>, not <cstdlib>

#include "vcl_compiler.h"
#include "vcl_cmath.h"
#include <cstdlib>

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
