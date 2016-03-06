#ifndef vcl_typeinfo_h_
#define vcl_typeinfo_h_

#include "vcl_compiler.h"
#include <typeinfo>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_type_info std::type_info
#define vcl_bad_cast std::bad_cast
#define vcl_bad_typeid std::bad_typeid

#endif // vcl_typeinfo_h_
