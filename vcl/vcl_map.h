#ifndef vcl_map_h_
#define vcl_map_h_

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_functional.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_map.h>
#else
# ifdef __GNUC__
#  include <map>
#  define vcl_map map
# else
#  include <map>
#  define vcl_map std::map
# endif
#endif

#define VCL_MAP_INSTANTIATE(T1, T2, L) extern "please include vcl_map.txx"

#endif
