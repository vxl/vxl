#ifndef vbl_shared_pointer_txx_
#define vbl_shared_pointer_txx_
/*
  fsm
*/
#include "vbl_shared_pointer.h"

#undef VBL_SHARED_POINTER_INSTANTIATE
#define VBL_SHARED_POINTER_INSTANTIATE(T) \
template struct vbl_shared_pointer<T >

#endif
