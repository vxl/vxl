#include "vcl_iterator.h"

#define VCL_COPY_INSTANTIATE(Inp, Out) \
template Out std::copy(Inp, Inp, Out);

#define VCL_SWAP_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(void swap(T&, T&));
 
#define VCL_OPERATOR_NE_INSTANTIATE(T) \
VCL_INSTANTIATE_INLINE(bool operator!=(T const&, T const &));

#define VCL_CONTAINABLE_INSTANTIATE(T) \
VCL_INSTANTIATE_INLINE(void construct(T *, T const &)); \
VCL_INSTANTIATE_INLINE(void destroy(T *));

#define VCL_LESS_INSTANTIATE(T) \
template struct vcl_less<T >;

#define VCL_FIND_INSTANTIATE(I, T) \
template I vcl_find(I, I, T const&);

// I is a random access iterator
#define VCL_SORT_INSTANTIATE(Rai) \
template void vcl_sort(Rai, Rai);
