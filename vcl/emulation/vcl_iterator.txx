#ifndef vcl_emulation_iterator_txx_
#define vcl_emulation_iterator_txx_
#include <vcl/vcl_compiler.h>
#include "vcl_iterator.h"
#include "vcl_algorithm.txx"

#define VCL_TAGS_INSTANTIATE(I, TAG)\
VCL_INSTANTIATE_INLINE(TAG iterator_category(I const &))

#define VCL_ITER_FWD_INSTANTIATE(ForwardIterator)\
VCL_OPERATOR_NE_INSTANTIATE(ForwardIterator);\
VCL_TAGS_INSTANTIATE(ForwardIterator, forward_iterator_tag)

#define VCL_ITER_BD_INSTANTIATE_Distance(BidirectionalIterator, Distance)\
VCL_INSTANTIATE_INLINE(void advance(BidirectionalIterator &, Distance));\
VCL_INSTANTIATE_INLINE(void __advance(BidirectionalIterator &, Distance, bidirectional_iterator_tag));\
VCL_INSTANTIATE_INLINE(void distance(BidirectionalIterator, BidirectionalIterator, Distance &));\
VCL_INSTANTIATE_INLINE(void __distance(BidirectionalIterator, BidirectionalIterator const &, Distance &, bidirectional_iterator_tag))

#define VCL_ITER_BD_INSTANTIATE(BidirectionalIterator)\
VCL_ITER_BD_INSTANTIATE_Distance(BidirectionalIterator, BidirectionalIterator::difference_type);\
VCL_OPERATOR_NE_INSTANTIATE(BidirectionalIterator);\
VCL_TAGS_INSTANTIATE(BidirectionalIterator, bidirectional_iterator_tag)

#define VCL_ITER_RA_INSTANTIATE_Distance(RandomAccessIterator, Distance)\
VCL_INSTANTIATE_INLINE(void advance(RandomAccessIterator &, Distance));\
VCL_INSTANTIATE_INLINE(void __advance(RandomAccessIterator &, Distance, random_access_iterator_tag));\
VCL_INSTANTIATE_INLINE(void distance(RandomAccessIterator, RandomAccessIterator, Distance &));\
VCL_INSTANTIATE_INLINE(void __distance(RandomAccessIterator const&, RandomAccessIterator const &, Distance &, random_access_iterator_tag))

#define VCL_ITER_RA_INSTANTIATE(RandomAccessIterator)\
INSTANTIATE_ITER_RA_Distance(RandomAccessIterator, ptrdiff_t);\
VCL_OPERATOR_NE_INSTANTIATE(RandomAccessIterator);\
VCL_TAGS_INSTANTIATE(RandomAccessIterator, random_access_iterator_tag)

#define VCL_ITER_BD_Distance_INSTANTIATE(BidirectionalIterator, Distance)\
VCL_INSTANTIATE_INLINE(void advance(BidirectionalIterator &, Distance));\
VCL_INSTANTIATE_INLINE(void __advance(BidirectionalIterator &, Distance, bidirectional_iterator_tag));\
VCL_INSTANTIATE_INLINE(void distance(BidirectionalIterator, BidirectionalIterator, Distance &));\
VCL_INSTANTIATE_INLINE(void __distance(BidirectionalIterator, BidirectionalIterator const &, Distance &, bidirectional_iterator_tag))

// Inp is a random access iterator.
// this works for vcl_vector<double>::iterator with gcc 2.7 and irix6-CC-n32 :
#undef VCL_SORT_INSTANTIATE
#define VCL_SORT_INSTANTIATE(Inp) \
VCL_INSTANTIATE_INLINE(void sort(Inp, Inp))

#undef VCL_COPY_INSTANTIATE
#define VCL_COPY_INSTANTIATE(Inp, Out)\
VCL_INSTANTIATE_INLINE(Out copy(Inp, Inp, Out))

#undef VCL_COPY_BACKWARD_INSTANTIATE
#define VCL_COPY_BACKWARD_INSTANTIATE(Inp, Out)\
VCL_INSTANTIATE_INLINE(Out copy_backward(Inp, Inp, Out))

#undef VCL_FIND_INSTANTIATE
#define VCL_FIND_INSTANTIATE(Inp,Out)\
VCL_INSTANTIATE_INLINE(Inp find(Inp, Inp, Out const&))

#endif // vcl_emulation_iterator_txx_
