#ifndef vcl_vector_h_
#define vcl_vector_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_vector.h>

#elif defined(__GNUC__)
# include_next <vector.h>
# define vcl_vector vector

#else
# include <vector>
# define vcl_vector std::vector
#endif

// Need to include vcl_vector.txx to create instances !
#define VCL_VECTOR_INSTANTIATE "error: include <vcl/vcl_vector.txx>"

#endif
