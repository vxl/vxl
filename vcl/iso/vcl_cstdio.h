#ifndef vcl_iso_cstdio_h_
#define vcl_iso_cstdio_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <cstdio>

// printf
#ifndef vcl_printf
#define vcl_printf std::printf
#endif
// sprintf
#ifndef vcl_sprintf
#define vcl_sprintf std::sprintf
#endif
// fprintf
#ifndef vcl_fprintf
#define vcl_fprintf std::fprintf
#endif
// vprintf
#ifndef vcl_vprintf
#define vcl_vprintf std::vprintf
#endif
// vsprintf
#ifndef vcl_vsprintf
#define vcl_vsprintf std::vsprintf
#endif
// vfprintf
#ifndef vcl_vfprintf
#define vcl_vfprintf std::vfprintf
#endif

//using STD::FILE;

#endif
