#ifndef vbl_quadruple_txx_
#define vbl_quadruple_txx_
/*
  fsm
*/
#include "vbl_quadruple.h"

//--------------------------------------------------------------------------------

#undef VBL_QUADRUPLE_INSTANTIATE
#define VBL_QUADRUPLE_INSTANTIATE(T1, T2, T3, T4) \
template struct vbl_quadruple<T1, T2, T3, T4 >; \
template vbl_quadruple<T1, T2, T3, T4 > vbl_make_quadruple(T1 const &, T2 const &, T3 const &, T4 const &)

#define VBL_QUADRUPLE_INSTANTIATE_comparisons(T1, T2, T3, T4) \
VCL_INSTANTIATE_INLINE( bool operator==(vbl_quadruple<T1, T2, T3, T4 > const &, vbl_quadruple<T1, T2, T3, T4 > const &) ); \
VCL_INSTANTIATE_INLINE( bool operator< (vbl_quadruple<T1, T2, T3, T4 > const &, vbl_quadruple<T1, T2, T3, T4 > const &) )

#endif
