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

//fsm: the instantiation macro for find() needs to instantiate find(I, I, T, tag)
//however, there seems to be no way to get the iterator category of I other than
//using iterator_traits<I>::iterator_category. the problem then is that find()
//is only defined for input_iterators and random_access_iterators. since a
//bidirectional_iterator_tag is an input_iterator, the following should be harmless.
template <class _BdIter, class _Tp>
inline _BdIter find(_BdIter __first,
                    _BdIter __last,
                    _Tp const & __val,
                    bidirectional_iterator_tag)
{
  return ::find(__first, __last, __val, input_iterator_tag());
}

#define VCL_FIND_INSTANTIATE(I, T) \
template <int N> struct fsm_find_tickler; /* empty template */ \
template <> struct fsm_find_tickler<__LINE__> { void method(I, I, T const &); }; \
void fsm_find_tickler<__LINE__>::method(I b, I e, T const &v) { find(b, e, v); } \
template I find(I, I, T const&); \
template I find(I, I, T const&, iterator_traits<I >::iterator_category)

#define VCL_FIND_IF_INSTANTIATE_ITER(I, P) \
template I find_if(I, I, P)

#define VCL_FIND_IF_INSTANTIATE(I, P) \
VCL_FIND_IF_INSTANTIATE_ITER(I, P); \
template I find_if<I,P>(I, I, P, random_access_iterator_tag)

#define VCL_REMOVE_INSTANTIATE(I, T) \
template I remove(I, I, T const &)

#endif
