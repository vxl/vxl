#ifndef vcl_string_h_
#define vcl_string_h_

#include "vcl_compiler.h"
#include <string>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_algorithm.h"
//vcl alias names to std names
#define vcl_char_traits std::char_traits
#define vcl_basic_string std::basic_string
#define vcl_string std::string
#define vcl_wstring std::wstring
#define vcl_getline std::getline

#endif // vcl_string_h_
