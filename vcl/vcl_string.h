//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_string_h_
#define vcl_string_h_

#include "vcl_compiler.h"

#if defined(VCL_GCC)
# include "iso/vcl_string.h"
# undef  vcl_char_traits
# define vcl_char_traits   string_char_traits
#else
# include "iso/vcl_string.h"
#endif

#define VCL_BASIC_STRING_INSTANTIATE \
extern "include vcl_string.txx instead"

#if VCL_USE_IMPLICIT_TEMPLATES
# include "vcl_string.txx"
#endif

#endif // vcl_string_h_
