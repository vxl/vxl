#ifndef vbl_array_txx_
#define vbl_array_txx_
/*
  fsm@robots.ox.ac.uk
*/
#include "vbl_array.h"

#undef VBL_ARRAY_INSTANTIATE
#define VBL_ARRAY_INSTANTIATE(T) \
template struct vbl_array<T >;

#endif
