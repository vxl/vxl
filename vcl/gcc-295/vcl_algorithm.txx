#ifndef vcl_gcc295_algorithm_txx_
#define vcl_gcc295_algorithm_txx_

#include <vcl/vcl_compiler.h>

#define VCL_SWAP_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(void swap(T&, T&))
 
#define VCL_OPERATOR_NE_INSTANTIATE(T)

#define VCL_CONTAINABLE_INSTANTIATE(T)
 
#define VCL_LESS_INSTANTIATE(T)\
template struct vcl_less<T >

#endif // vcl_gcc295_algorithm_txx_
