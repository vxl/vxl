// This is vcl/vcl_limits.cxx

//:
// \file
// \author IMS

#include "vcl_compiler.h"

#if !VCL_CXX_HAS_HEADER_LIMITS || defined(VCL_GCC_295)
# include  "gcc-295/vcl_limits.cxx"
#endif
