#ifndef vcl_vector_h_
#define vcl_vector_h_

#include "vcl_compiler.h"

// -------------------- emulation
#if !VCL_USE_NATIVE_STL
# include "emulation/vcl_vector.h"

// -------------------- sunpro 5.0
#elif defined(VCL_SUNPRO_CC_50)
# include "sunpro/vcl_vector.h"

// -------------------- iso
#else
# include "iso/vcl_vector.h"
#endif

#if defined(VCL_ICC)
# include "vcl_utility.h"
// Intel compiler's std::swap can't handle the special bit-iterator of
// vector<bool>
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

#if VCL_USE_IMPLICIT_TEMPLATES
# include "vcl_vector.txx"
#endif

#endif // vcl_vector_h_
