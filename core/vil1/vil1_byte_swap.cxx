// This is core/vil1/vil1_byte_swap.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil1_byte_swap.h"
#include <vcl_cassert.h>
#include <vcl_cstddef.h>

// this is not efficient
void vil1_byte_swap(void *b_, void *e_)
{
  char *b = static_cast<char*>(b_);
  char *e = static_cast<char*>(e_);
  assert(b < e);
  vcl_ptrdiff_t n = e-b;
  for (vcl_ptrdiff_t i=0; i<n/2; ++i) {
    char tmp = b[i];
    b[i] = b[n-1-i];
    b[n-1-i] = tmp;
  }
}
