#include <vcl/vcl_compiler.h>
#include "vcl_map.h"
#include "vcl_multimap.h"

#include "vcl_rbtree.txx"
#include "vcl_hash_map.txx"

// --- Vcl_Map ---

// * You can't call VCL_MAP_INSTANTIATE twice from within the same macro
// as the __LINE__ will be the same.

#undef VCL_MAP_INSTANTIATE
#define VCL_MAP_INSTANTIATE(T, Key, Comp)			\
template class vcl_map<T, Key, Comp VCL_DFL_TMPL_ARG(vcl_alloc) >;	\
template class vcl_multimap<T,Key,Comp VCL_DFL_TMPL_ARG(vcl_alloc) >;	\
VCL_RBTREE_MAP_PAIR_INSTANTIATE(T, Key, __LINE__)

// This "vcl_identity" passthru gets __LINE__ expanded
#define VCL_RBTREE_MAP_PAIR_INSTANTIATE(T, Key, TAG) VCL_RBTREE_MAP_PAIRx_INSTANTIATE(T, Key, TAG)

#define VCL_RBTREE_MAP_PAIRx_INSTANTIATE(T, Key, TAG)	\
typedef vcl_pair<T const, Key > RBPairc ## TAG;		\
VCL_PAIR_const_INSTANTIATE(T const, Key);		\
VCL_RBTREE_PAIR_INSTANTIATE(T, RBPairc ## TAG)

#define VCL_RBTREE_PAIR_INSTANTIATE(T, RBPair)		\
VCL_RBTREE_INSTANTIATE(T,RBPair,vcl_select1st<RBPair >,vcl_less<T >);\
VCL_RBTREE_VALUE_INSTANTIATE(RBPair)

