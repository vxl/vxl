#ifndef vcl_gcc_295_algorithm_txx_
#define vcl_gcc_295_algorithm_txx_

#include <vcl/vcl_algorithm.h>

#define VCL_SWAP_INSTANTIATE(T)\
VCL_INSTANTIATE_INLINE(void swap(T&, T&))
 
#define VCL_OPERATOR_NE_INSTANTIATE(T)

#define VCL_CONTAINABLE_INSTANTIATE(T)
 
// I is a random access iterator
#define VCL_SORT_INSTANTIATE(I, T) \
template void __final_insertion_sort(I, I); \
template void __introsort_loop(I, I, I, int); \
template void __introsort_loop(I, I, I, long)

#define VCL_SORT_INSTANTIATE_CMP(I, T, C) \
/* fix as needed */

#define VCL_COPY_INSTANTIATE(Inp, Out) \
template Out copy(Inp, Inp, Out)

#define VCL_FIND_INSTANTIATE(I, T) \
template <int N> struct fsm_find_tickler; /* empty template */ \
template <> struct fsm_find_tickler<__LINE__> { void method(I, I, T const &); }; \
void fsm_find_tickler<__LINE__>::method(I b, I e, T const &v) { find(b, e, v); } \
/*template I find(I, I, T const &, iterator_traits<I >::iterator_category);*/ \
template I find(I, I, T const &);

#define VCL_FIND_IF_INSTANTIATE(I, P) \
template I find_if(I, I, P);

#define VCL_REMOVE_INSTANTIATE(I, T) \
template I remove(I, I, T const &);


#endif
