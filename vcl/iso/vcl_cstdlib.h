#ifndef vcl_iso_cstdlib_h_
#define vcl_iso_cstdlib_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <cstdlib>

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
// abs
#ifndef vcl_abs
#define vcl_abs std::abs
#endif
// abort
#ifndef vcl_abort
#define vcl_abort std::abort
#endif
// qsort
#ifndef vcl_qsort
#define vcl_qsort std::qsort
#endif
// free
#ifndef vcl_free
#define vcl_free std::free
#endif
// malloc
#ifndef vcl_malloc
#define vcl_malloc std::malloc
#endif
// strtol
#ifndef vcl_strtol
#define vcl_strtol std::strtol
#endif
// strtoul
#ifndef vcl_strtoul
#define vcl_strtoul std::strtoul
#endif
// realloc
#ifndef vcl_realloc
#define vcl_realloc std::realloc
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
