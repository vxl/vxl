#ifndef vil_byte_swap_h_
#define vil_byte_swap_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

void vil_byte_swap(void *, void *);

template <class T>
inline void vil_byte_swap(T &x) { vil_byte_swap(&x, &x + 1); }

#endif
