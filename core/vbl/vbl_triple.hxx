#ifndef vbl_triple_hxx_
#define vbl_triple_hxx_
/*
  fsm
*/
#include "vbl_triple.h"

#undef VBL_TRIPLE_INSTANTIATE
#define VBL_TRIPLE_INSTANTIATE(T1, T2, T3) \
template struct vbl_triple<T1, T2, T3 >; \
template vbl_triple<T1, T2, T3 > vbl_make_triple(T1 const &, T2 const &, T3 const &)

#define VBL_TRIPLE_INSTANTIATE_comparisons(T1, T2, T3) \
/*template  bool operator==(vbl_triple<T1, T2, T3 > const &, vbl_triple<T1, T2, T3 > const &)  ; */ \
/*template  bool operator< (vbl_triple<T1, T2, T3 > const &, vbl_triple<T1, T2, T3 > const &)  */

#endif
