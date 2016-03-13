#ifndef vcl_csignal_h_
#define vcl_csignal_h_

#include "vcl_compiler.h"
#include <csignal>
//vcl alias names to std names
#define vcl_sig_atomic_t std::sig_atomic_t
#define vcl_raise std::raise
#define vcl_signal std::signal

#endif // vcl_csignal_h_
