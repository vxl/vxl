// FIXME:
// VCL_VECTOR_INSTANTIATE is there, the rest of the instantiations
// have the wrong name and should be moved to other files.
//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_sunpro_map_txx_
#define vcl_sunpro_map_txx_

#include <vcl/vcl_map.h>
#include <vcl/vcl_compiler.h>

#if 0 // has member templates
#define VCL_MAP_INSTANTIATE_MT_InputIterator(maptype, T, Key, Comp, InputIterator)\
template maptype<T, Key, Comp >::maptype(InputIterator, InputIterator);\
template maptype<T, Key, Comp >::maptype(InputIterator first, InputIterator last, Comp const&);\
template void maptype<T, Key, Comp >::insert(InputIterator first, InputIterator last)

#else
#define VCL_MAP_INSTANTIATE_MT_InputIterator(maptype, T, Key, Comp, InputIterator) /* no-op */
#endif

#define VCL_MAP_INSTANTIATE_ITERATOR(InputIterator, Distance)\
template void __distance (InputIterator , InputIterator , Distance& , std::bidirectional_iterator_tag ) 



#define VCL_MAP_INSTANTIATE(T, Key, Comp)\
template class vcl_map<T, Key, Comp >; \
template class vcl_map<T, Key, Comp >::__rep_type; \
VCL_MAP_INSTANTIATE_ITERATOR(vcl_map<T VCL_COMMA Key VCL_COMMA Comp >::iterator, unsigned); \
VCL_MAP_INSTANTIATE_MT_InputIterator(vcl_map, T, Key, Comp, vcl_map<T VCL_COMMA Key VCL_COMMA Comp >::iterator)

#endif
