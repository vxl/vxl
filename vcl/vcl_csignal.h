#ifndef vcl_csignal_h_
#define vcl_csignal_h_

#include "vcl_compiler.h"
#include <csignal>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_sig_atomic_t std::sig_atomic_t
#define vcl_raise std::raise
#define vcl_signal std::signal

#endif // vcl_csignal_h_
