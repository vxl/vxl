#ifndef vcl_vector_h_
#define vcl_vector_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_vector.h>

#elif defined(VCL_GCC_WITH_GNU_LIBSTDCXX_V2)
#  include_next <vector.h>
#  define vcl_vector vector
#else
# include <vector>
# define vcl_vector std::vector
#endif

#endif
