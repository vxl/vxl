// This is vxl/vbl/vbl_hash_map.txx
#ifndef vbl_hash_map_txx_
#define vbl_hash_map_txx_

#include "vbl_hash_map.h"
#include <vcl_compiler.h>

#ifndef VCL_WIN32
#undef VBL_HASHTABLE_INSTANTIATE
#define VBL_HASHTABLE_INSTANTIATE(K, T) \
template class vbl_hash_map<K, T >
#else
#undef VBL_HASHTABLE_INSTANTIATE
#define VBL_HASHTABLE_INSTANTIATE(K, T) /* */
#endif

#endif
