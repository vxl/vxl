#ifndef vcl_iso_csignal_h_
#define vcl_iso_csignal_h_

#include <csignal>

// sig_atomic_t
#ifndef vcl_sig_atomic_t
#define vcl_sig_atomic_t std::sig_atomic_t
#endif
// raise
#ifndef vcl_raise
#define vcl_raise std::raise
#endif
// signal
#ifndef vcl_signal
#define vcl_signal std::signal
#endif

endif
