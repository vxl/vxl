// This is vxl/vil/vil_byte_swap.h
#ifndef vil_byte_swap_h_
#define vil_byte_swap_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

void vil_byte_swap(void *, void *);

template <class T>
inline void vil_byte_swap(T &x) { vil_byte_swap(&x, &x + 1); }

#endif // vil_byte_swap_h_
