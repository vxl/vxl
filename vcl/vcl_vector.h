#ifndef vcl_vector_h_
#define vcl_vector_h_

#include <vcl/vcl_compiler.h>

// -------------------- emulation
#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_vector.h>

// -------------------- gcc with old library
#elif defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
# include <vector.h>
# define vcl_vector vector

// -------------------- iso
#else
# include <vector>
# define vcl_vector std::vector
#endif

#endif
