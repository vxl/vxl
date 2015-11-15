#ifndef vcl_queue_h_
#define vcl_queue_h_

#include "vcl_compiler.h"
#include "iso/vcl_queue.h"

#define VCL_QUEUE_INSTANTIATE(T) extern "you must #include vcl_queue.txx"
#define VCL_PRIORITY_QUEUE_INSTANTIATE(T) extern "you must #include vcl_queue.txx"

#if VCL_USE_IMPLICIT_TEMPLATES
# include "vcl_queue.txx"
#endif

#endif // vcl_queue_h_
