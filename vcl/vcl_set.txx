#include <vcl/vcl_set.h>

#include <vcl/vcl_compiler.h>

#undef VCL_SET_INSTANTIATE

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_set.txx>
#else
# if defined(VCL_EGCS)
#  include <vcl/egcs/vcl_set.txx>
# elif defined(VCL_GCC_295)
#  include <vcl/gcc-295/vcl_set.txx>
# elif defined(VCL_SUNPRO_CC)
#  include <vcl/sunpro/vcl_set.txx>
# elif defined(VCL_SGI_CC)
#  include <vcl/sgi/vcl_set.txx>
# elif defined(VCL_WIN32)
#  include <vcl/win32/vcl_set.txx>
# else
   error "USE_NATIVE_STL with unknown compiler"
# endif
#endif
