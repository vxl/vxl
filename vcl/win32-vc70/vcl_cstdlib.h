#ifndef vcl_vc_cstdlib_h_
#define vcl_vc_cstdlib_h_

#include <cstdlib>

#define vcl_generic_cstdlib_STD std

namespace std {
  inline long abs(long x) { return x >= 0 ? x : -x; }
}

#include "../generic/vcl_cstdlib.h"

#endif // vcl_cstdlib_h_
