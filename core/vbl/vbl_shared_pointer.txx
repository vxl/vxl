#ifndef vbl_shared_pointer_txx_
#define vbl_shared_pointer_txx_
/*
  fsm@robots.ox.ac.uk
*/
#include "vbl_shared_pointer.h"

#undef VUL_SHARED_POINTER_INSTANTIATE
#define VUL_SHARED_POINTER_INSTANTIATE(T) \
template struct vbl_shared_pointer<T >

#endif
