// -*- c++ -*-
#include <vcl/vcl_iterator.h>

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_iterator.txx>
#elif defined(VCL_EGCS)
# include <vcl/egcs/vcl_iterator.txx>
#elif defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-295/vcl_iterator.txx>
#elif defined(VCL_GCC_295) && defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-libstdcxx-v3/vcl_iterator.txx>
#elif defined(VCL_SUNPRO_CC)
# include <vcl/sunpro/vcl_iterator.txx>
#elif defined(VCL_SGI_CC)
# include <vcl/sgi/vcl_iterator.txx>
#elif defined(VCL_WIN32)
# include <vcl/win32/vcl_iterator.txx>
#else
# include <vcl/iso/vcl_iterator.txx>
#endif
