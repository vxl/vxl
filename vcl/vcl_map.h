#ifndef vcl_map_h_
#define vcl_map_h_

#include "vcl_compiler.h"

// vcl_less<> is a default argument to vcl_map<> and vcl_multimap<>
// so we need this for compilers where vcl_less is a macro.
#include "vcl_functional.h"
#include "iso/vcl_map.h"

#define VCL_MAP_INSTANTIATE(T1, T2, L) extern "please include vcl_map.txx"
#define VCL_MULTIMAP_INSTANTIATE(T1, T2, L) extern "please include vcl_multimap.txx"

#if VCL_USE_IMPLICIT_TEMPLATES
# include "vcl_map.txx"
#endif

#endif // vcl_map_h_
