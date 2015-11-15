#ifndef vcl_vector_h_
#define vcl_vector_h_

#include "vcl_compiler.h"
#include "iso/vcl_vector.h"

// Intel compiler's std::swap can't handle the special bit-iterator of
// vector<bool>. But, on systems with newer GCCs, the Intel compiler
// can use the GNU library, which doesn't have this problem.

#define VCL_VECTOR_INSTANTIATE extern "you must include vcl_vector.txx first"

#if VCL_USE_IMPLICIT_TEMPLATES
# include "vcl_vector.txx"
#endif

#endif // vcl_vector_h_
