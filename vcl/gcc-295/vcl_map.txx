#ifndef vcl_gcc295_map_txx_
#define vcl_gcc295_map_txx_

#include <vcl/vcl_map.h>
#include <vcl/vcl_compiler.h>

#define VCL_MAP_INSTANTIATE_MT_InputIterator(maptype, Key, T, Comp, InputIterator)\
template maptype<Key, T, Comp >::maptype(InputIterator, InputIterator);\
template maptype<Key, T, Comp >::maptype(InputIterator first, InputIterator last, Comp const&);\
template void maptype<Key, T, Comp >::insert(InputIterator first, InputIterator last)


//template class _Rb_tree<Key, pair<Key const, T>, _Select1st<pair<Key const, T> >, Comp >;

#define VCL_MAP_INSTANTIATE(Key, T, Comp)\
template class vcl_map<Key, T, Comp >; \
VCL_MAP_INSTANTIATE_MT_InputIterator(vcl_map, Key, T, Comp, vcl_map<Key VCL_COMMA T VCL_COMMA Comp >::iterator)

#endif
