//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_sunpro_map_txx_
#define vcl_sunpro_map_txx_

#include <vcl/vcl_map.h>
#include <vcl/vcl_memory.h>  // vcl_allocator<>
#include <vcl/vcl_utility.h> // vcl_pair<>

#if 0 // has member templates
#define VCL_MAP_INSTANTIATE_MT_InputIterator(maptype, Key, T, Comp, Alloc, InputIterator)\
template maptype<Key, T, Comp, Alloc >::maptype(InputIterator, InputIterator);\
template maptype<Key, T, Comp, Alloc >::maptype(InputIterator first, InputIterator last, Comp const&);\
template void maptype<Key, T, Comp, Alloc >::insert(InputIterator first, InputIterator last)
#else
#define VCL_MAP_INSTANTIATE_MT_InputIterator(maptype, Key, T, Comp, Alloc, InputIterator) /* no-op */
#endif


#define VCL_MAP_INSTANTIATE_ITERATOR(InputIterator, Distance)\
template void std :: __distance (InputIterator , InputIterator , Distance& , std::bidirectional_iterator_tag ) 


//
#define VCL_MAP_INSTANTIATE_internal(tag, Key, T, Comp)\
typedef vcl_map<Key, T, Comp, vcl_allocator<vcl_pair<Key const,T > > > grik##tag; \
template class grik##tag; \
template class grik##tag :: __rep_type; \
template void std::__distance(grik##tag::iterator, grik##tag::iterator, unsigned &, std::bidirectional_iterator_tag); \
/*VCL_MAP_INSTANTIATE_MT_InputIterator(vcl_map, Key, T, Comp, vcl_map<Key VCL_COMMA T VCL_COMMA Comp >::iterator)*/

// the purpose of this macro is to expand out 'tag', before passing it on.
#define VCL_MAP_INSTANTIATE_internal_expandline(tag, Key, T, Comp) \
VCL_MAP_INSTANTIATE_internal(tag, Key, T, Comp)

// clients call this
#undef VCL_MAP_INSTANTIATE
#define VCL_MAP_INSTANTIATE(Key, T, Comp) \
VCL_MAP_INSTANTIATE_internal_expandline(__LINE__, Key, T, Comp)


//-------------------- multimap
#undef VCL_MULTIMAP_INSTANTIATE
#define VCL_MULTIMAP_INSTANTIATE(Key, T, Comp)\
template class vcl_multimap<Key, T, Comp >;

#endif
