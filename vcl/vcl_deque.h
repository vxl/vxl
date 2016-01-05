#ifndef vcl_deque_h_
#define vcl_deque_h_

#include "vcl_compiler.h"

#include "iso/vcl_deque.h"

#define VCL_DEQUE_INSTANTIATE \
extern "include vcl_deque.txx instead"

#if VCL_USE_IMPLICIT_TEMPLATES
# include "vcl_deque.txx"
#endif

#endif // vcl_deque_h_
