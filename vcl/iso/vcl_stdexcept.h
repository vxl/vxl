#ifndef vcl_iso_stdexcept_h_
#define vcl_iso_stdexcept_h_

#include <stdexcept>

// logic_error
#ifndef vcl_logic_error
#define vcl_logic_error std::logic_error
#endif
// domain_error
#ifndef vcl_domain_error
#define vcl_domain_error std::domain_error
#endif
// invalid_argument
#ifndef vcl_invalid_argument
#define vcl_invalid_argument std::invalid_argument
#endif
// length_error
#ifndef vcl_length_error
#define vcl_length_error std::length_error
#endif
// out_of_range
#ifndef vcl_out_of_range
#define vcl_out_of_range std::out_of_range
#endif
// runtime_error
#ifndef vcl_runtime_error
#define vcl_runtime_error std::runtime_error
#endif
// range_error
#ifndef vcl_range_error
#define vcl_range_error std::range_error
#endif
// overflow_error
#ifndef vcl_overflow_error
#define vcl_overflow_error std::overflow_error
#endif
// underflow_error
#ifndef vcl_underflow_error
#define vcl_underflow_error std::underflow_error
#endif

#endif // vcl_iso_stdexcept_h_
