//-*- c++ -*-
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_algorithm.h>

#undef VCL_SWAP_INSTANTIATE
#define VCL_SWAP_INSTANTIATE(T) \
VCL_INSTANTIATE_INLINE(void swap(T&, T&));

#undef VCL_OPERATOR_NE_INSTANTIATE
#define VCL_OPERATOR_NE_INSTANTIATE(T) \
VCL_INSTANTIATE_INLINE(bool operator!=(T const&, T const &))

#define VCL_CONTAINABLE_INSTANTIATE(T) \
VCL_INSTANTIATE_INLINE(void construct(T *, T const &)); \
VCL_INSTANTIATE_INLINE(void destroy(T *));
//VCL_SWAP_INSTANTIATE(T);

//SGI CC does not allow explicit instantiation of inlines.
//So use "VCL_INSTANTIATE_INLINE" instead of "template".
#define VCL_FIND_INSTANTIATE(I, T) \
VCL_INSTANTIATE_INLINE( I find(I, I, T const&) );

// I is a random access iterator.
// this works for vector<double>::iterator with gcc 2.7 and irix6-CC-n32 :
#define VCL_SORT_INSTANTIATE(I) \
VCL_INSTANTIATE_INLINE( void sort(I, I ) );

#undef VCL_COPY_INSTANTIATE
#define VCL_COPY_INSTANTIATE(Inp, Out) \
VCL_INSTANTIATE_INLINE(Out copy(Inp, Inp, Out));

#undef VCL_COPY_BACKWARD_INSTANTIATE
#define VCL_COPY_BACKWARD_INSTANTIATE(Inp, Out) \
VCL_INSTANTIATE_INLINE(Out copy_backward(Inp, Inp, Out));
