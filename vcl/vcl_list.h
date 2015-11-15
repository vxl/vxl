#ifndef vcl_list_h_
#define vcl_list_h_

#include "vcl_compiler.h"
# include "iso/vcl_list.h"

#define VCL_LIST_INSTANTIATE \
extern "include vcl_list.txx instead"

#if VCL_USE_IMPLICIT_TEMPLATES
# include "vcl_list.txx"
#endif

#endif // vcl_list_h_
