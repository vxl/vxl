#ifndef vcl_emulation_rbtree_txx_
#define vcl_emulation_rbtree_txx_
#include <vcl/vcl_compiler.h>
#include "vcl_algorithm.txx"
#include "vcl_iterator.txx"
#include "vcl_pair.txx"

/////////////////////////////////////////////////////////////////////////////

// --- RB TREE ---
#undef VCL_RBTREE_INSTANTIATE
#define VCL_RBTREE_INSTANTIATE(Key, Value, GetKey, Compare)	\
template class rb_tree<Key, Value, GetKey, Compare, vcl_alloc >

#define VCL_RBTREE_VALUE_INSTANTIATE(Value)\
template class __rb_tree_base<Value, vcl_alloc >;\
template struct __rb_tree_iterator<Value >;\
template struct __rb_tree_const_iterator<Value >;\
template class vcl_simple_alloc<__rb_tree_node<Value >, vcl_alloc >;\
VCL_ITER_BD_INSTANTIATE_Distance(__rb_tree_iterator<Value >, size_t);\
VCL_ITER_BD_INSTANTIATE_Distance(__rb_tree_const_iterator<Value >, size_t);\
VCL_OPERATOR_NE_INSTANTIATE(__rb_tree_const_iterator<Value >);\
VCL_OPERATOR_NE_INSTANTIATE(__rb_tree_iterator<Value >);\
VCL_PAIR_INSTANTIATE(__rb_tree_iterator<Value >, __rb_tree_iterator<Value > );\
VCL_PAIR_INSTANTIATE(__rb_tree_const_iterator<Value >, __rb_tree_const_iterator<Value > );\
VCL_PAIR_INSTANTIATE(__rb_tree_const_iterator<Value >, bool );\
VCL_PAIR_INSTANTIATE(__rb_tree_iterator<Value >, bool );\
template class reverse_bidirectional_iterator<__rb_tree_const_iterator<Value >, Value, Value const &, ptrdiff_t>;\
template class reverse_bidirectional_iterator<__rb_tree_iterator<Value >, Value, Value &, ptrdiff_t>;\
VCL_SWAP_INSTANTIATE(__rb_tree_node<Value > *)

#endif // vcl_emulation_rbtree_txx_
