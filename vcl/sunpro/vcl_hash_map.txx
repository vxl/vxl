#ifndef vcl_sunpro_hash_map_txx_
#define vcl_sunpro_hash_map_txx_
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_hash_map.h>

#define VCL_HASH_MAP_INSTANTIATE(Key, Value, Hash, Comp) \
template class vcl_hash_map<Key, Value, Hash, Comp > ; \
template class vcl_hash_map<Key, Value, Hash, Comp > :: iterator; \
template class vcl_hash_map<Key, Value, Hash, Comp > :: const_iterator;

#endif
