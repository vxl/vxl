#ifndef vcl_emulation_algorithm_txx_
#define vcl_emulation_algorithm_txx_
#include <vcl/vcl_compiler.h>
#include "vcl_algorithm.h"

#undef VCL_SWAP_INSTANTIATE
#define VCL_SWAP_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(void swap(T&,T&))

#undef VCL_OPERATOR_NE_INSTANTIATE
#define VCL_OPERATOR_NE_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(bool operator!=(T const&,T const&))

#undef VCL_CONTAINABLE0_INSTANTIATE
#define VCL_CONTAINABLE0_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(void construct(T*,T const&));\
VCL_INSTANTIATE_INLINE(void destroy(T*))

#undef VCL_CONTAINABLE_INSTANTIATE
#define VCL_CONTAINABLE_INSTANTIATE(T)\
VCL_CONTAINABLE0_INSTANTIATE(T);\
VCL_SWAP_INSTANTIATE(T)

// --- Unary templates ---
// Templates with one type mentioned, no requirements on type

#undef VCL_LESS_INSTANTIATE
#define VCL_LESS_INSTANTIATE(T)\
template struct vcl_less<T >;\
VCL_UNARY_INSTANTIATE(vcl_less<T >)

#undef VCL_UNARY_INSTANTIATE
#define VCL_UNARY_INSTANTIATE(T)\
template struct vcl_identity<T >

#endif // vcl_emulation_algorithm_txx_
