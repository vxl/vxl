// -*- c++ -*-
/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_functional.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_functional.txx>
#else
# if defined(VCL_EGCS)
#  include <vcl/egcs/vcl_functional.txx>
# elif defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
#  include <vcl/gcc-295/vcl_functional.txx>
# elif defined(VCL_GCC_295) && defined(GNU_LIBSTDCXX_V3)
#  include <vcl/gcc-libstdcxx-v3/vcl_functional.txx>
# elif defined(VCL_SUNPRO_CC)
#  include <vcl/sunpro/vcl_functional.txx>
# elif defined(VCL_SGI_CC)
#  include <vcl/sgi/vcl_functional.txx>
# elif defined(VCL_WIN32)
#  include <vcl/win32/vcl_functional.txx>
# else
   error "USE_NATIVE_STL with unknown compiler"
# endif
#endif
