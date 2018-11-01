// This is core/vil1/vil1_byte_swap.h
#ifndef vil1_byte_swap_h_
#define vil1_byte_swap_h_
//:
// \file
// \author fsm

void vil1_byte_swap(void *, void *);

template <class T>
inline void vil1_byte_swap(T &x) { vil1_byte_swap(&x, &x + 1); }

#endif // vil1_byte_swap_h_
