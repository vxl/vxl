#ifndef vcl_egcs_map_txx_
#define vcl_egcs_map_txx_

#include <vcl/vcl_map.h>
#include <vcl/vcl_compiler.h>

// * You can't call INSTANTIATE_MAP twice from within the same macro
// as the __LINE__ will be the same.

#ifdef __STL_MEMBER_TEMPLATES
#define VCL_MAP_INSTANTIATE_MT_InputIterator(maptype, T, Key, Comp, InputIterator)\
template maptype<T, Key, Comp >::maptype(InputIterator, InputIterator);\
template maptype<T, Key, Comp >::maptype(InputIterator first, InputIterator last, Comp const&);\
template void maptype<T, Key, Comp >::insert(InputIterator first, InputIterator last);
#else
#define VCL_MAP_INSTANTIATE_MT_InputIterator(maptype, T, Key, Comp, InputIterator) /* no-op */
#endif


#define VCL_MAP_INSTANTIATE(T, Key, Comp)\
template class vcl_map<T, Key, Comp >; \
VCL_MAP_INSTANTIATE_MT_InputIterator(vcl_map, T, Key, Comp, vcl_map<T VCL_COMMA Key VCL_COMMA Comp >::iterator)

// #ifdef __STL_MEMBER_TEMPLATES
// #define INSTANTIATE_MAP_MT_InputIterator(maptype, T, Key, Comp, InputIterator)
// template maptype<T, Key, Comp >::maptype(InputIterator, InputIterator);
// template maptype<T, Key, Comp >::maptype(InputIterator first, InputIterator last, Comp const&);
// template void maptype<T, Key, Comp >::insert(InputIterator first, InputIterator last)
// #else
// #define INSTANTIATE_MAP_MT_InputIterator(maptype, T, Key, Comp, InputIterator) /* no-op */
// #endif


// #define INSTANTIATE_MAP(T, Key, Comp)
// template class map<T, Key, Comp >;
// template class map<T, Key, Comp >::rep_type;
// template class multimap<T, Key, Comp >;
// INSTANTIATE_MAP_MT_InputIterator(map, T, Key, Comp, map<T VCL_COMMA Key VCL_COMMA Comp >::iterator)

#endif
