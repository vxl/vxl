#ifndef vcl_rel_ops_h_
#define vcl_rel_ops_h_
/*
  fsm@robots.ox.ac.uk
*/

// Purpose: To provide a portable way of getting inline
// function templates
//      operator!=      ... !(x == y)
//      operator>       ...  (y <  x)
//      operator<=      ... !(y <  x)
//      operator>=      ... !(x <  y)
// For ISO compilers, these live in namespace std::rel_ops.

#include "vcl_compiler.h"

#if !VCL_USE_NATIVE_STL
# include "emulation/vcl_rel_ops.h"

#elif defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
// this header is wrong. v2 of the GNU library is wrong wrong wrong.
# include "vcl_functional.h"

#elif defined(VCL_SGI_CC)
# include "vcl_utility.h"
using std::operator!=;
using std::operator> ;
using std::operator<=;
using std::operator>=;

#else // -------------------- ISO
# include "vcl_utility.h"
using std::rel_ops::operator!=;
using std::rel_ops::operator> ;
using std::rel_ops::operator<=;
using std::rel_ops::operator>=;
#endif

// instantiation macro for compilers that need it.
#define VCL_REL_OPS_INSTANTIATE(T) \
VCL_INSTANTIATE_INLINE(bool operator!=(T const &, T const &)); \
VCL_INSTANTIATE_INLINE(bool operator> (T const &, T const &)); \
VCL_INSTANTIATE_INLINE(bool operator<=(T const &, T const &)); \
VCL_INSTANTIATE_INLINE(bool operator>=(T const &, T const &));

#endif // vcl_rel_ops_h_
