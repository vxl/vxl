#ifndef vcl_iso_cstdlib_h_
#define vcl_iso_cstdlib_h_

#include <cstdlib>

// abs
#ifndef vcl_abs
#define vcl_abs std::abs
#endif
// div
#ifndef vcl_div
#define vcl_div std::div
#endif
// labs
#ifndef vcl_labs
#define vcl_labs std::labs
#endif
// ldiv
#ifndef vcl_ldiv
#define vcl_ldiv std::ldiv
#endif
// srand
#ifndef vcl_srand
#define vcl_srand std::srand
#endif
// rand
#ifndef vcl_rand
#define vcl_rand std::rand
#endif
// atexit
#ifndef vcl_atexit
#define vcl_atexit std::atexit
#endif
// getenv
#ifndef vcl_getenv
#define vcl_getenv std::getenv
#endif
// system
#ifndef vcl_system
#define vcl_system std::system
#endif
// exit
#ifndef vcl_exit
#define vcl_exit std::exit
#endif
// abort
#ifndef vcl_abort
#define vcl_abort std::abort
#endif
// size_t
#ifndef vcl_size_t
#define vcl_size_t std::size_t
#endif
// qsort
#ifndef vcl_qsort
#define vcl_qsort std::qsort
#endif
// calloc
#ifndef vcl_calloc
#define vcl_calloc std::calloc
#endif
// malloc
#ifndef vcl_malloc
#define vcl_malloc std::malloc
#endif
// free
#ifndef vcl_free
#define vcl_free std::free
#endif
// realloc
#ifndef vcl_realloc
#define vcl_realloc std::realloc
#endif
// atol
#ifndef vcl_atol
#define vcl_atol std::atol
#endif
// atof
#ifndef vcl_atof
#define vcl_atof std::atof
#endif
// atoi
#ifndef vcl_atoi
#define vcl_atoi std::atoi
#endif
// mblen
#ifndef vcl_mblen
#define vcl_mblen std::mblen
#endif
// mbstowcs
#ifndef vcl_mbstowcs
#define vcl_mbstowcs std::mbstowcs
#endif
// mbtowc
#ifndef vcl_mbtowc
#define vcl_mbtowc std::mbtowc
#endif
// strtod
#ifndef vcl_strtod
#define vcl_strtod std::strtod
#endif
// strtol
#ifndef vcl_strtol
#define vcl_strtol std::strtol
#endif
// strtoul
#ifndef vcl_strtoul
#define vcl_strtoul std::strtoul
#endif
// wctomb
#ifndef vcl_wctomb
#define vcl_wctomb std::wctomb
#endif
// wcstombs
#ifndef vcl_wcstombs
#define vcl_wcstombs std::wcstombs
#endif

using std::exit;
using std::abort;
using std::size_t;
using std::abs;
using std::abort;
using std::qsort;
using std::free;
using std::malloc;
using std::strtol;
using std::strtoul;
using std::realloc;

#endif
