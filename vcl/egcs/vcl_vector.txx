// FIXME:
// VCL_VECTOR_INSTANTIATE is there, the rest of the instantiations
// have the wrong name and should be moved to other files.
//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_egcs_vector_txx_
#define vcl_egcs_vector_txx_

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_pair.txx>

// --- Iterators ---

#define INSTANTIATE_TAGS(I, TAG)\
VCL_INSTANTIATE_INLINE(TAG iterator_category(I const &))

#define INSTANTIATE_ITER_FWD(ForwardIterator)\
INSTANTIATE_OPERATOR_NE(ForwardIterator);\
INSTANTIATE_TAGS(ForwardIterator, forward_iterator_tag)

#define INSTANTIATE_ITER_BD_Distance(BidirectionalIterator, Distance)\
VCL_INSTANTIATE_INLINE(void advance(BidirectionalIterator &, Distance));\
VCL_INSTANTIATE_INLINE(void __advance(BidirectionalIterator &, Distance, bidirectional_iterator_tag));\
VCL_INSTANTIATE_INLINE(void distance(BidirectionalIterator, BidirectionalIterator, Distance &));\
VCL_INSTANTIATE_INLINE(void __distance(BidirectionalIterator, BidirectionalIterator const &, Distance &, bidirectional_iterator_tag))

#define INSTANTIATE_ITER_BD(BidirectionalIterator)\
INSTANTIATE_ITER_BD_Distance(BidirectionalIterator, BidirectionalIterator::difference_type);\
INSTANTIATE_OPERATOR_NE(BidirectionalIterator);\
INSTANTIATE_TAGS(BidirectionalIterator, bidirectional_iterator_tag)

#define INSTANTIATE_ITER_RA_Distance(RandomAccessIterator, Distance)\
VCL_INSTANTIATE_INLINE(void advance(RandomAccessIterator &, Distance));\
VCL_INSTANTIATE_INLINE(void __advance(RandomAccessIterator &, Distance, random_access_iterator_tag));\
VCL_INSTANTIATE_INLINE(void distance(RandomAccessIterator, RandomAccessIterator, Distance &));\
VCL_INSTANTIATE_INLINE(void __distance(RandomAccessIterator const&, RandomAccessIterator const &, Distance &, random_access_iterator_tag))

#define INSTANTIATE_ITER_RA(RandomAccessIterator)\
INSTANTIATE_ITER_RA_Distance(RandomAccessIterator, ptrdiff_t);\
INSTANTIATE_OPERATOR_NE(RandomAccessIterator);\
INSTANTIATE_TAGS(RandomAccessIterator, random_access_iterator_tag)


#define VCL_FIND_INSTANTIATE(I, T) template I find(I, I, T const &)

#define INSTANTIATE_COPY(I, O) // VCL_INSTANTIATE_INLINE_LOOP(O copy(I, I, O))

#define INSTANTIATE_COPY_BACKWARD(I, O)

// --- List ---

#ifdef __STL_MEMBER_TEMPLATES
#define INSTANTIATE_LIST_MT_InputIterator(T, InputIterator)\
template list<T >::list(InputIterator first, InputIterator last);\
template void list<T >::insert(list<T >::iterator position, InputIterator first, InputIterator last);\
template void list<T >::range_initialize(InputIterator first, InputIterator last)
#else
#define INSTANTIATE_LIST_MT_InputIterator(T, InputIterator) /* no-op */
#endif

#define INSTANTIATE_LIST(T) \
template class list<T >;\
INSTANTIATE_LIST_MT_InputIterator(T, list<T >::const_iterator)


#define STLINST_uninitialized_copy(Inp, Fwd, Size) \
template Fwd __uninitialized_copy_aux(Inp, Inp, Fwd, __false_type);\
template pair<Inp, Fwd> __uninitialized_copy_n(Inp, Size, Fwd, input_iterator_tag);\
VCL_INSTANTIATE_INLINE_LOOP(Fwd uninitialized_copy(Inp, Inp, Fwd))

// --- Vector ---
#undef VCL_VECTOR_INSTANTIATE
#define VCL_VECTOR_INSTANTIATE(T) \
template vector<T >::iterator __uninitialized_fill_n_aux(vector<T >::iterator, vector<T >::size_type, T const &, __false_type);\
/*VCL_FIND_INSTANTIATE(vector<T >::iterator, T)*/;\
template vector<T >::iterator fill_n(vector<T >::iterator, vector<T >::size_type, T const &);\
INSTANTIATE_COPY(vector<T >::const_iterator, vector<T >::iterator);\
STLINST_uninitialized_copy(vector<T >::const_iterator, vector<T >::iterator, vector<T >::size_type);\
template class vector<T >

/////////////////////////////////////////////////////////////////////////////
// --- Set ---

#define INSTANTIATE_SET(T, Comp)\
template class set<T, Comp >; \
template class multiset<T, Comp >

/////////////////////////////////////////////////////////////////////////////

// --- Map ---

// * You can't call INSTANTIATE_MAP twice from within the same macro
// as the __LINE__ will be the same.

#ifdef __STL_MEMBER_TEMPLATES
#define INSTANTIATE_MAP_MT_InputIterator(maptype, T, Key, Comp, InputIterator)\
template maptype<T, Key, Comp >::maptype(InputIterator, InputIterator);\
template maptype<T, Key, Comp >::maptype(InputIterator first, InputIterator last, Comp const&);\
template void maptype<T, Key, Comp >::insert(InputIterator first, InputIterator last)
#else
#define INSTANTIATE_MAP_MT_InputIterator(maptype, T, Key, Comp, InputIterator) /* no-op */
#endif


#define INSTANTIATE_MAP(T, Key, Comp)\
template class map<T, Key, Comp >;\
template class map<T, Key, Comp >::rep_type;\
template class multimap<T, Key, Comp >;\
INSTANTIATE_MAP_MT_InputIterator(map, T, Key, Comp, map<T VCL_COMMA Key VCL_COMMA Comp >::iterator)

/////////////////////////////////////////////////////////////////////////////

// --- Hash Map ---

// * You can't call INSTANTIATE_HASHMAP twice from within the same macro
// as the __LINE__ will be the same.

// This macro ensures that the type is usable as a key for a hash map.
#define INSTANTIATE_HASHKEY(Key) \
template struct equal_to<Key>

// Then this macro may be used to instantiate the specific hashmaps.
#define INSTANTIATE_HASHMAP(Key, Value, Hash, Comp)\
template class hash_map<Key, Value, Hash, Comp INSTANTIATE_alloc >; \
template class hash_multimap<Key, Value, Hash, Comp INSTANTIATE_alloc >

/////////////////////////////////////////////////////////////////////////////


// --- HASH TABLE ---
#define INSTANTIATE_HASHTABLE(Key, Value, GetKey, Hash, Compare, TAG)\
template class hashtable<Value, Key, Hash, GetKey, Compare, alloc >

/////////////////////////////////////////////////////////////////////////////
// --- SORT ---

// I is a random access iterator
#define INSTANTIATE_SORT(I) \
template void __final_insertion_sort(I, I); \
template void __introsort_loop(I, I, I, int)

#endif
