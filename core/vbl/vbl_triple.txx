#ifndef vbl_triple_txx_
#define vbl_triple_txx_
/*
  fsm@robots.ox.ac.uk
*/
#include "vbl_triple.h"

#undef VUL_TRIPLE_INSTANTIATE
#define VUL_TRIPLE_INSTANTIATE(T1, T2, T3) \
template struct vbl_triple<T1, T2, T3 >; \
template vbl_triple<T1, T2, T3 > vbl_make_triple(T1 const &, T2 const &, T3 const &)

#define VUL_TRIPLE_INSTANTIATE_comparisons(T1, T2, T3) \
VCL_INSTANTIATE_INLINE( bool operator==(vbl_triple<T1, T2, T3 > const &, vbl_triple<T1, T2, T3 > const &) ); \
VCL_INSTANTIATE_INLINE( bool operator< (vbl_triple<T1, T2, T3 > const &, vbl_triple<T1, T2, T3 > const &) )

#endif
