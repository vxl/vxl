
#include <vcl/vcl_compiler.h>

#define VCL_COPY_INSTANTIATE(Inp, Out) \
template Out std::copy(Inp, Inp, Out)

#define VCL_SWAP_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(void swap(T&, T&))
 
#define VCL_OPERATOR_NE_INSTANTIATE(T)

#define VCL_CONTAINABLE_INSTANTIATE(T)
 
#define VCL_LESS_INSTANTIATE(T)\
template struct vcl_less<T >;

#define VCL_FIND_INSTANTIATE(I, T) \
template I std::find(I, I, T const&);

// I is a random access iterator
#define VCL_SORT_INSTANTIATE(I) \
template void __final_insertion_sort(I, I); \
template void __introsort_loop(I, I, I, int);
