// This is vxl/vil/vil_byte_swap.cxx

/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_byte_swap.h"
#include <vcl_cassert.h>

// this is not efficient
void vil_byte_swap(void *b_, void *e_) {
  char *b = static_cast<char*>(b_);
  char *e = static_cast<char*>(e_);
  assert(b < e);
  unsigned n = e-b;
  for (unsigned i=0; i<n/2; ++i) {
    char tmp = b[i];
    b[i] = b[n-1-i];
    b[n-1-i] = tmp;
  }
}

#if 0 // for 2.7
VCL_INSTANTIATE_INLINE( void vil_byte_swap(long &) );
VCL_INSTANTIATE_INLINE( void vil_byte_swap(unsigned short &) );
VCL_INSTANTIATE_INLINE( void vil_byte_swap(unsigned long &) );
#endif
