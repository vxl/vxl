#ifndef vcl_set_txx_
#define vcl_set_txx_

#include <vcl/vcl_set.h>

#undef VCL_SET_INSTANTIATE

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_set.txx>
#elif defined(VCL_EGCS)
# include <vcl/egcs/vcl_set.txx>
#elif defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-295/vcl_set.txx>
#elif defined(VCL_GCC_295) && defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-libstdcxx-v3/vcl_set.txx>
#elif defined(VCL_SUNPRO_CC)
# include <vcl/sunpro/vcl_set.txx>
#elif defined(VCL_SGI_CC)
# include <vcl/sgi/vcl_set.txx>
#elif defined(VCL_WIN32)
# include <vcl/win32/vcl_set.txx>
#else
# include <vcl/iso/vcl_set.txx>
#endif

#endif
