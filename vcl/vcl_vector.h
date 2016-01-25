#ifndef vcl_vector_h_
#define vcl_vector_h_

#include "vcl_compiler.h"

// -------------------- iso
#include "iso/vcl_vector.h"

// Intel compiler's std::swap can't handle the special bit-iterator of
// vector<bool>. But, on systems with newer GCCs, the Intel compiler
// can use the GNU library, which doesn't have this problem.
#if defined(VCL_ICC_8) && defined(__INTEL_CXXLIB_ICC)
# include "vcl_utility.h"
namespace std {
  inline void swap(std::vector<bool, std::allocator<bool> >::iterator::reference a,
                   std::vector<bool, std::allocator<bool> >::iterator::reference b)
  {
    bool tmp = a;
    a = b;
    b = tmp;
  }
} // end namespace std
#endif

#define VCL_VECTOR_INSTANTIATE extern "you must include vcl_vector.txx first"

#include "vcl_vector.txx"

#endif // vcl_vector_h_
