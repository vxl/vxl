//-*- c++ -*-------------------------------------------------------------------
#ifndef VCL_string_h_
#define VCL_string_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_string.h>
#else
# ifdef __GNUC__
// gcc-2.95:
#  include <string>
#  define vcl_string string

# else
// not a gcc
#  include <string>
#  define vcl_string std::string

#  ifdef VCL_VC60
#  include <vcl/vcl_iostream.h>
   inline ostream& operator<<(ostream& os, vcl_string const& s) {
     return os << s.c_str();
   }
#  endif

# endif
# if defined(VCL_GCC_EGCS) || defined(VCL_SUNPRO_CC)
#   define VCL_STRING_IS_TYPEDEF 1
# endif
#endif

#endif
