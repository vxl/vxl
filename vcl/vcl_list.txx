#include <vcl/vcl_list.h>

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_list.txx>
#else
# if defined(VCL_EGCS)
#  include <vcl/egcs/vcl_list.txx>
# elif defined(VCL_GCC_295)
#  include <vcl/gcc-295/vcl_list.txx>
# elif defined(VCL_SUNPRO_CC)
#  include <vcl/sunpro/vcl_list.txx>
# elif defined(VCL_SGI_CC)
#  include <vcl/sgi/vcl_list.txx>
# elif defined(VCL_WIN32)
#  include <vcl/win32/vcl_list.txx>
# else
   error "USE_NATIVE_STL with unknown compiler"
# endif
#endif
