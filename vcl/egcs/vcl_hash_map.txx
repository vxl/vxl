#ifndef vcl_egcs_hash_map_txx_
#define vcl_egcs_hash_map_txx_
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_hash_map.h>

#if 0
  // actually, this is a bad idea if hash<T> is a non-trivial class.
  // fsm@robots. linux-egcs will complain about statement with no effect in the function
  // 'void swap(hash<T> &, hash<T> &)'. That's because assigning a hash<T> to a hash<T>
  // has no effect, so the warning is harmless. But it's annoying, so we add this inline
  // specialization :
# include <vcl/vcl_algorithm.h>
  template <class T>
  inline void swap/*<hash<T> >*/(hash<T> &, hash<T> &) { }
#endif

// --- Hash Map ---

// * You can't call HASH_MAP_INSTANTIATE twice from within the same macro
// as the __LINE__ will be the same.

// This macro ensures that the type is usable as a key for a hash map.
#define INSTANTIATE_HASHKEY(Key) \
template struct equal_to<Key>

// Then this macro may be used to instantiate the specific hashmaps.
#define VCL_HASH_MAP_INSTANTIATE(Key, Value, Hash, Comp) \
template class hash_map<Key, Value, Hash, Comp >; \
template class hash_multimap<Key, Value, Hash, Comp >;\
template class __hashtable_iterator<pair<Key const, Value >,\
				 Key,\
				 hash<Key >,\
				 select1st<pair<Key const, Value > >, \
				 equal_to<Key > , \
				 __default_alloc_template<true, 0> >;\
template class __hashtable_iterator<pair<Key const, Value >,\
				 Key,\
				 hash<Key >,\
				 select1st<pair<Key const, Value > >, \
				 equal_to<Key > , \
				 __default_alloc_template<0, 0> >;


// // --- Hash Map ---

// // * You can't call INSTANTIATE_HASHMAP twice from within the same macro
// // as the __LINE__ will be the same.

// // This macro ensures that the type is usable as a key for a hash map.
// #define INSTANTIATE_HASHKEY(Key) \
// template struct equal_to<Key>

// // Then this macro may be used to instantiate the specific hashmaps.
// #define INSTANTIATE_HASHMAP(Key, Value, Hash, Comp)\
// template class hash_map<Key, Value, Hash, Comp INSTANTIATE_alloc >; \
// template class hash_multimap<Key, Value, Hash, Comp INSTANTIATE_alloc >

// /////////////////////////////////////////////////////////////////////////////


// // --- HASH TABLE ---
// #define INSTANTIATE_HASHTABLE(Key, Value, GetKey, Hash, Compare, TAG)\
// template class hashtable<Value, Key, Hash, GetKey, Compare, alloc >

#endif
