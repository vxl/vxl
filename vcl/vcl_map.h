#ifndef vcl_map_h_
#define vcl_map_h_

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_functional.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_map.h>
# include <vcl/emulation/vcl_multimap.h>

#elif defined(__GNUC__)
# include <map>
# define vcl_map map
# define vcl_multimap multimap

#elif defined(VCL_SUNPRO_CC)
# include <map>
# define vcl_map std::map
# define vcl_multimap std::multimap

#elif defined(VCL_WIN32)
# include <map>
# define vcl_map std::map
# define vcl_multimap std::multimap

#else
# include <map>
# define vcl_map std::map
# define vcl_multimap std::multimap
#endif

#define VCL_MAP_INSTANTIATE(T1, T2, L) extern "please include vcl_map.txx"
#define VCL_MULTIMAP_INSTANTIATE(T1, T2, L) extern "please include vcl_multimap.txx"

#endif
