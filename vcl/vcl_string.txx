#include <vcl/vcl_string.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_string.txx>
#elif defined(VCL_EGCS)
# include <vcl/egcs/vcl_string.txx>
#elif defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-295/vcl_string.txx>
#elif defined(VCL_GCC_295) && defined(GNU_LIBSTDCXX_V3)
# include <vcl/gcc-libstdcxx-v3/vcl_string.txx>
#elif defined(VCL_SUNPRO_CC)
# include <vcl/iso/vcl_string.txx>
#elif defined(VCL_SGI_CC)
# include <vcl/sgi/vcl_string.txx>
#elif defined(VCL_WIN32)
# include <vcl/win32/vcl_string.txx>
#else
# include <vcl/iso/vcl_string.txx>
#endif
