// This is vxl/vil/vil_byte_swap.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil_byte_swap.h"
#include <vcl_cassert.h>

// this is not efficient
void vil_byte_swap(void *b_, void *e_) {
  char *b = static_cast<char*>(b_);
  char *e = static_cast<char*>(e_);
  assert(b < e);
  unsigned long n = e-b;
  for (unsigned long i=0; i<n/2; ++i) {
    char tmp = b[i];
    b[i] = b[n-1-i];
    b[n-1-i] = tmp;
  }
}
