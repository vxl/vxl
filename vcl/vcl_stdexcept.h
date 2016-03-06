#ifndef vcl_stdexcept_h_
#define vcl_stdexcept_h_

#include "vcl_compiler.h"
#include <stdexcept>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_logic_error std::logic_error
#define vcl_domain_error std::domain_error
#define vcl_invalid_argument std::invalid_argument
#define vcl_length_error std::length_error
#define vcl_out_of_range std::out_of_range
#define vcl_runtime_error std::runtime_error
#define vcl_range_error std::range_error
#define vcl_overflow_error std::overflow_error
#define vcl_underflow_error std::underflow_error

#endif // vcl_stdexcept_h_
