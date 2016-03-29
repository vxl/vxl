// This is brl/bseg/bvpl/bvpl_octree/bvpl_octree_vector_operator.h
#ifndef bvpl_octree_vector_operator_h
#define bvpl_octree_vector_operator_h
//:
// \file
// \brief An abstract class with a template operation (as in template method pattern) to apply a vector of bvpl_kernel on an octree
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 17, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_octree_kernel_operator.h"
#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/functors/bvpl_local_max_functor.h>
#include <boct/boct_tree.h>


//: A class with a template operation (as in template method pattern) to apply a vector of bvpl_kernel on an octree
//  The class also contains abstract methods to be specialized in the subclasses
template <class T_data>
class bvpl_octree_vector_operator
{
 public:

  //: Applies a vector of kernels and functor to a grid. Returns a grid that combines the responses of the kernels as specified in the subclass
  template <class F>
  bool operate(boct_tree<short ,T_data>* tree_in,
               F functor,
               bvpl_kernel_vector_sptr kernel_vector,
               boct_tree<short ,bvpl_octree_sample<T_data> >* tree_out,
               short level, double cell_length)
  {
    bvpl_local_max_functor<T_data> func_max;
    boct_tree<short ,T_data>* temp_tree = tree_in->clone();
    bvpl_octree_kernel_operator<T_data> oper(tree_in);
    for (unsigned int id = 0; id < kernel_vector->kernels_.size(); ++id)
    {
      temp_tree->init_cells(func_max.min_response());
      bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
      oper.operate(functor, kernel, temp_tree,level, cell_length);
      combine_kernel_responses(tree_out, temp_tree, id);
    }

    return true;
  }

 protected:
  //: Abstract function to be implemented on children on how to combine current kernel response with output response

  virtual void combine_kernel_responses(boct_tree<short ,bvpl_octree_sample<T_data> >* tree_out, boct_tree<short ,T_data>* tree_curr,
                                        int id_curr)=0;

  //: Compares response grids and stores in out, the maximum responses. This function gets called in subclasses when implementing combine_kernel_responses

  void keep_max_response_tree(boct_tree<short ,bvpl_octree_sample<T_data> >* tree_out, boct_tree<short ,T_data>* new_tree,
                              int new_id);
};


//: Compares response grids and stores in grid1, the wining responses.
template<class T_data>
void bvpl_octree_vector_operator<T_data>::keep_max_response_tree(boct_tree<short ,bvpl_octree_sample<T_data> >* tree_out,
                                                                 boct_tree<short ,T_data>* new_tree,
                                                                 int new_id)
{
  bvpl_local_max_functor<T_data> func_max;

  std::vector<boct_tree_cell<short,T_data>*> new_leaves = new_tree->leaf_cells();
  std::vector<boct_tree_cell<short,bvpl_octree_sample<T_data> >*> leaves_out = tree_out->leaf_cells();

  for (unsigned i=0; i< new_leaves.size(); i++)
  {
    T_data val1 = new_leaves[i]->data();
    T_data val2 = leaves_out[i]->data().response_;
    if (func_max.greater_than(val1, val2))
      leaves_out[i]->set_data(bvpl_octree_sample<T_data>(new_leaves[i]->data(), new_id));
  }
}

#endif
