#ifndef vnl_copy_h_
#define vnl_copy_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// purpose: easy conversion between vectors and matrices templated
// over different types.

template <class S, class T>
void vnl_copy(S const *src, T *dst, unsigned n);

template <class S, class T>
void vnl_copy(S const &, T &);

#endif
