// -*- c++ -*-
#ifndef vbl_hash_map_txx_
#define vbl_hash_map_txx_

#include <vbl/vbl_hash_map.h>

#undef VBL_HASHTABLE_INSTANTIATE
#define VBL_HASHTABLE_INSTANTIATE(K, T) \
template class vbl_hash_map<K, T >;

#endif
