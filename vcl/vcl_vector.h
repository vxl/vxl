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

// -------------------- sunpro 5.0
#elif defined(VCL_SUNPRO_CC_50)
# include <vcl/sunpro/vcl_vector.h>
# define vcl_vector vcl_vector_sunpro_50

// -------------------- iso
#else
# include <vector>
# define vcl_vector std::vector
#endif

#endif
