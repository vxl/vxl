#ifndef vbl_array_1d_txx_
#define vbl_array_1d_txx_
/*
  fsm
*/
#include "vbl_array_1d.h"

#undef VBL_ARRAY_1D_INSTANTIATE
#define VBL_ARRAY_1D_INSTANTIATE(T) \
template struct vbl_array_1d<T >

#endif
