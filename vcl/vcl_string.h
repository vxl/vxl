//-*- c++ -*-------------------------------------------------------------------
#ifndef VCL_string_h_
#define VCL_string_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_string.h>

#elif defined(VCL_GCC_WITH_GNU_LIBSTDCXX_V2)
# include <string>
# define vcl_string string
# define vcl_string_char_traits string_char_traits

#elif defined(VCL_WIN32)
# include <string>
//fsm@robots: for some reason, vc60 crashes if vnl_fwd.h is used in conjunction
//with this #define. using a typedef seems to fix it.
//# define vcl_string std::string
typedef std::string vcl_string;
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

#define VCL_BASIC_STRING_INSTANTIATE \
extern "include vcl/vcl_string.txx instead"

#endif
