//-*- c++ -*-------------------------------------------------------------------
#ifndef VCL_string_h_
#define VCL_string_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_string.h>

#elif defined(__GNUC__)
# include <string>
# define vcl_string string
# define vcl_string_char_traits string_char_traits

#elif defined(VCL_WIN32)
# include <string>
# define vcl_string std::string
# define vcl_string_char_traits std::char_traits
#else
# include <string>
# define vcl_string std::string
# define vcl_string_char_traits std::string_char_traits
#endif

//--------------------------------------------------------------------------------

#if defined(VCL_VC60)
# include <vcl/vcl_iostream.h>
inline ostream& operator<<(ostream& os, vcl_string const& s) {
  return os << s.c_str();
}
#endif // VCL_VC60

// who needs to know this?
#if defined(VCL_GCC_EGCS) || defined(VCL_SUNPRO_CC)
# define VCL_STRING_IS_TYPEDEF 1
#endif

#endif
