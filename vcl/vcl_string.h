//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_string_h_
#define vcl_string_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_string.h>

#elif defined(VCL_GCC_WITH_GNU_LIBSTDCXX_V2)
# include <string>
# define vcl_basic_string  basic_string
# define vcl_char_traits   string_char_traits
# define vcl_string        string

#elif defined(VCL_WIN32)
# include <vcl/win32/vcl_string.h>

#else
# include <vcl/iso/vcl_string.h>
#endif

//// who needs to know this?
//#if defined(VCL_GCC_EGCS) || defined(VCL_SUNPRO_CC)
//# define VCL_STRING_IS_TYPEDEF 1
//#endif

#define VCL_BASIC_STRING_INSTANTIATE \
extern "include vcl/vcl_string.txx instead"

#endif // vcl_string_h_
