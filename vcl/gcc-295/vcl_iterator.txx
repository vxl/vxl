#ifndef vcl_gcc295_iterator_txx_
#define vcl_gcc295_iterator_txx_

#include <algo.h>

// Inp is a random access iterator
#define VCL_SORT_INSTANTIATE(Inp) \
template void __final_insertion_sort(Inp, Inp); \
template void __introsort_loop(Inp, Inp, Inp, int)

#define VCL_COPY_INSTANTIATE(Inp, Out) // template Out std::copy(Inp, Inp, Out)

#define VCL_FIND_INSTANTIATE(Inp, T) // template Inp std::find(Inp, Inp, T const&)

#endif
