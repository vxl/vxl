// This is brl/bseg/bvpl/bvpl_octree/bvpl_octree_vector_operator_impl.h
#ifndef bvpl_octree_vector_operator_impl_h
#define bvpl_octree_vector_operator_impl_h
//:
// \file
// \brief Subclasses that implement the abstract interface of bvpl_octree_vector_operator. They can be used to queue operations
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 30, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_octree_vector_operator.h"


//: Class to apply a vector of kernels and functor to a grid, returning a grid containing at each voxel the max response
template <class T_data>
class bvpl_max_vector_operator: public bvpl_octree_vector_operator<T_data>
{
 public:
  void combine_kernel_responses(boct_tree<short ,bvpl_octree_sample <T_data> >* tree_out, boct_tree<short ,T_data>* tree_curr,
                                        int id_curr) override
  {
    this->keep_max_response_tree(tree_out, tree_curr, id_curr);
  }
};

#endif // bvpl_octree_vector_operator_impl_h
