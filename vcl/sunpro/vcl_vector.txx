#ifndef vcl_sunpro_vector_txx_
#define vcl_sunpro_vector_txx_

#include <vcl/vcl_pair.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_compiler.h>

#define VCL_VECTOR_STLINST_uninitialized_copy(Inp, Fwd, Size) \
template Fwd std::copy(Inp, Inp, Fwd);\
template Fwd std::copy_backward(Inp, Inp, Fwd)

// --- Vector ---
#define VCL_VECTOR_INSTANTIATE(T) \
template class vcl_vector<T >;\
template void std::fill(vcl_vector<T >::iterator, vcl_vector<T >::iterator, T const&); \
template T *std::copy(T *, T *, T *); \
template T *std::copy(T const *, T const *, T *); \
template T *std::copy_backward(T *, T *, T *);

//VCL_VECTOR_STLINST_uninitialized_copy(vcl_vector<T >::const_iterator, vcl_vector<T >::iterator, vcl_vector<T >::size_type);\
//VCL_VECTOR_STLINST_uninitialized_copy(vcl_vector<T >::iterator, vcl_vector<T >::iterator, vcl_vector<T >::size_type);\

#endif
