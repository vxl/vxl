#ifndef vbl_quadruple_hxx_
#define vbl_quadruple_hxx_
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
/*template  bool operator==(vbl_quadruple<T1, T2, T3, T4 > const &, vbl_quadruple<T1, T2, T3, T4 > const &)  ; */ \
/*template  bool operator< (vbl_quadruple<T1, T2, T3, T4 > const &, vbl_quadruple<T1, T2, T3, T4 > const &)  */

#endif
