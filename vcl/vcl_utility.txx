#ifndef vcl_utility_txx_
#define vcl_utility_txx_
/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_utility.h>

#undef VCL_PAIR_INSTANTIATE

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_utility.txx>
#elif defined(VCL_EGCS)
# include <vcl/egcs/vcl_utility.txx>
#elif defined(VCL_GCC_295)
# include <vcl/gcc-295/vcl_utility.txx>
#elif defined(VCL_SUNPRO_CC)
# include <vcl/sunpro/vcl_utility.txx>
//#elif defined(VCL_SGI_CC)
//# include <vcl/sgi/vcl_utility.txx>
#elif defined(VCL_WIN32)
# include <vcl/win32/vcl_utility.txx>
#else
  error "USE_NATIVE_STL with unknown compiler"
#endif

#endif
