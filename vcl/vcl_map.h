#ifndef vcl_map_h_
#define vcl_map_h_

#include <vcl/vcl_compiler.h>

// vcl_less<> is a default argument to vcl_map<> and vcl_multimap<>
// so we need this for compilers where vcl_less is a macro.
#include <vcl/vcl_functional.h>

// -------------------- emulation
#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_map.h>
# include <vcl/emulation/vcl_multimap.h>

// -------------------- gcc with old library
#elif defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
# include <map.h>
# include <multimap.h>
# define vcl_map      map
# define vcl_multimap multimap

// -------------------- iso
#else
# include <map>
# define vcl_map      std::map
# define vcl_multimap std::multimap
#endif

#define VCL_MAP_INSTANTIATE(T1, T2, L) extern "please include vcl_map.txx"
#define VCL_MULTIMAP_INSTANTIATE(T1, T2, L) extern "please include vcl_multimap.txx"

#endif
