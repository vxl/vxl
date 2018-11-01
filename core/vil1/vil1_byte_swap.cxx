// This is core/vil1/vil1_byte_swap.cxx
//:
// \file
// \author fsm

#include <cstddef>
#include "vil1_byte_swap.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// this is not efficient
void vil1_byte_swap(void *b_, void *e_)
{
  char *b = static_cast<char*>(b_);
  char *e = static_cast<char*>(e_);
  assert(b < e);
  std::ptrdiff_t n = e-b;
  for (std::ptrdiff_t i=0; i<n/2; ++i) {
    char tmp = b[i];
    b[i] = b[n-1-i];
    b[n-1-i] = tmp;
  }
}
