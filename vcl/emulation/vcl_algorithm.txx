//-*- c++ -*-
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_algorithm.h>

#undef VCL_SWAP_INSTANTIATE
#define VCL_SWAP_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(void swap(T&, T&))

#undef VCL_OPERATOR_NE_INSTANTIATE
#define VCL_OPERATOR_NE_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(bool operator!=(T const&, T const &))

#define VCL_CONTAINABLE_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(void construct(T *, T const &))\
VCL_INSTANTIATE_INLINE(void destroy(T *)) \
VCL_SWAP_INSTANTIATE(T)

#define VCL_LESS_INSTANTIATE(T)\
template struct vcl_less<T >; \
VCL_UNARY_INSTANTIATE(vcl_less<T >)

#define VCL_UNARY_INSTANTIATE(T)\
template struct vcl_identity<T >;

#define VCL_FIND_INSTANTIATE(I, T) \
template I find(I, I, T const&);
