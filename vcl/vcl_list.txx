// -*- c++ -*-
#ifndef vcl_list_txx_
#define vcl_list_txx_

#include <vcl/vcl_list.h>

#undef VCL_LIST_INSTANTIATE

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_list.txx>
#elif defined(VCL_EGCS)
# include <vcl/egcs/vcl_list.txx>
#elif defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-295/vcl_list.txx>
#elif defined(VCL_GCC_295) && defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-libstdcxx-v3/vcl_list.txx>
#elif defined(VCL_SUNPRO_CC)
# include <vcl/sunpro/vcl_list.txx>
#elif defined(VCL_SGI_CC)
# include <vcl/sgi/vcl_list.txx>
#elif defined(VCL_WIN32)
# include <vcl/win32/vcl_list.txx>
#else
# include <vcl/iso/vcl_list.txx>
#endif

#endif
