#ifndef vcl_map_txx_
#define vcl_map_txx_

#include <vcl/vcl_map.h>

// It's vcl_map<Key, T, Compare = less<Key>, Allocator = allocator<pair<Key const, T> > >
// The key_type is Key
// The value_type is pair<Key const, T>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_map.txx>
#elif defined(VCL_EGCS)
# include <vcl/egcs/vcl_map.txx>
#elif defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-295/vcl_map.txx>
#elif defined(VCL_GCC_295) && defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-libstdcxx-v3/vcl_map.txx>
#elif defined(VCL_SUNPRO_CC)
# include <vcl/sunpro/vcl_map.txx>
#elif defined(VCL_SGI_CC)
# include <vcl/sgi/vcl_map.txx>
#elif defined(_MSC_VER)
# include <vcl/win32/vcl_map.txx>
#else
  error "USE_NATIVE_STL with unknown compiler"
#endif

#endif
