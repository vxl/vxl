#ifndef vcl_utility_h_
#define vcl_utility_h_

#include "vcl_compiler.h"

#include "iso/vcl_utility.h"

#define VCL_PAIR_INSTANTIATE extern "you must include vcl_utility.txx first"
#define VCL_PAIR_const_INSTANTIATE extern "you must include vcl_utility.txx first"

#if VCL_USE_IMPLICIT_TEMPLATES
# include "vcl_utility.txx"
#endif

#endif // vcl_utility_h_
