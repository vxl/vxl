#ifndef vcl_cctype_h_
#define vcl_cctype_h_

#include "vcl_compiler.h"
#include <cctype>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_isalnum std::isalnum
#define vcl_isdigit std::isdigit
#define vcl_isprint std::isprint
#define vcl_isupper std::isupper
#define vcl_tolower std::tolower
#define vcl_isalpha std::isalpha
#define vcl_isgraph std::isgraph
#define vcl_ispunct std::ispunct
#define vcl_isxdigit std::isxdigit
#define vcl_toupper std::toupper
#define vcl_iscntrl std::iscntrl
#define vcl_islower std::islower
#define vcl_isspace std::isspace

#endif // vcl_cctype_h_
