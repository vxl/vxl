#ifndef vcl_gcc_295_map_txx_
#define vcl_gcc_295_map_txx_

#include <vcl_map.h>

// The rb tree used by the map<> class appears to be
//   template <class _Key,
//             class _Value,
//             class _KeyOfValue,
//             class _Compare,
//             class _Alloc = __STL_DEFAULT_ALLOCATOR(_Value) > class _Rb_tree;
// where:
//   _Key = Key
//   _Value = pair<Key const, T>
//   _KeyOfValue = _Select1st<pair<Key const, T> >
//   _Compare = Comp
//   _Alloc = allocator<T>


// Macro to instantiate the underlying _Rb_tree and its member templates.
//template class _Rb_tree<Key, pair<Key const, T>, _Select1st<pair<Key const,T > >,Comp >;
#define VCL_MAP_INSTANTIATE_RB_TREE_tagged(tag, Key, T, Comp) \
template class _Rb_tree<Key, pair<Key const,T >, _Select1st<pair<Key const,T > >, Comp, allocator<T > >; \
typedef        _Rb_tree<Key, pair<Key const,T >, _Select1st<pair<Key const,T > >, Comp, allocator<T >  > cont##tag; \
template void cont##tag::insert_unique(cont##tag::iterator, cont##tag::iterator)
#define VCL_MAP_INSTANTIATE_RB_TREE_passthrough(tag, Key, T, Comp) \
VCL_MAP_INSTANTIATE_RB_TREE_tagged(tag, Key, T, Comp);
#define VCL_MAP_INSTANTIATE_RB_TREE(Key, T, Comp) \
VCL_MAP_INSTANTIATE_RB_TREE_passthrough(__LINE__, Key, T, Comp);


// Macro to instantiate something or other.
#define VCL_MAP_INSTANTIATE_MT_InputIterator(maptype, Key, T, Comp, InputIterator) \
template maptype<Key, T, Comp >::maptype(InputIterator, InputIterator); \
template maptype<Key, T, Comp >::maptype(InputIterator first, InputIterator last, Comp const&); \
template void maptype<Key, T, Comp >::insert(InputIterator first, InputIterator last);


// Macro to instantiate a vcl_map.
#undef VCL_MAP_INSTANTIATE
#define VCL_MAP_INSTANTIATE(Key, T, Comp) \
template class vcl_map<Key, T, Comp >; \
VCL_MAP_INSTANTIATE_MT_InputIterator(vcl_map, Key, T, Comp, vcl_map<Key VCL_COMMA T VCL_COMMA Comp >::iterator); \
VCL_MAP_INSTANTIATE_RB_TREE(Key, T, Comp);


// Macro to instantiate a vcl_multimap.
#undef VCL_MULTIMAP_INSTANTIATE
#define VCL_MULTIMAP_INSTANTIATE(Key, T, Comp) \
template class vcl_multimap<Key, T, Comp >;

#endif
