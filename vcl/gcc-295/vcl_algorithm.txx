#ifndef vcl_gcc295_algorithm_txx_
#define vcl_gcc295_algorithm_txx_

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_algorithm.h>

#define VCL_SWAP_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(void swap(T&, T&))
 
#define VCL_OPERATOR_NE_INSTANTIATE(T)

#define VCL_CONTAINABLE_INSTANTIATE(T)
 
// Inp is a random access iterator
#define VCL_SORT_INSTANTIATE(Inp) \
template void __final_insertion_sort(Inp, Inp); \
template void __introsort_loop(Inp, Inp, Inp, int)

#define VCL_COPY_INSTANTIATE(Inp, Out) \
template Out copy(Inp, Inp, Out)

#define VCL_FIND_INSTANTIATE(Inp, T) \
template Inp find(Inp, Inp, T const&)

#endif
