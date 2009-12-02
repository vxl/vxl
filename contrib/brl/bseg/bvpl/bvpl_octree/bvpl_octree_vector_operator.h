// This is brl/bseg/bvpl/bvpl_octree/bvpl_octree_vector_operator.h
#ifndef bvpl_octree_vector_operator_h
#define bvpl_octree_vector_operator_h
//:
// \file
// \brief  An abstract class with a template operation (as in template method pattern) to apply a vector of bvpl_kernel on a octree
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 17, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_octree_kernel_operator.h"
#include <bvpl/bvpl_kernel.h>
#include <bvpl/bvpl_local_max_functor.h>
#include <boct/boct_tree.h>


// :A class with a template operation (as in template method pattern) to apply a vector of bvpl_kernel on a octree
//  The class also contains abstract methods to be specialized in the subclasses
template <class T_data>
class bvpl_octree_vector_operator
{
 public:

  //: Applies a vector of kernels and functor to a grid. Returns a grid that combines the responses of the kernels as specified in the subclass
  template <class F>
  bool operate(F functor,
               boct_tree<short ,T_data>* tree_in,
               bvpl_kernel_vector_sptr kernel_vector,
               boct_tree<short ,T_data>* response_tree,
               boct_tree<short ,int>* id_tree, short level, double cell_length)
  {
    bvpl_local_max_functor<T_data> func_max;
    boct_tree<short ,T_data>* temp_tree = response_tree->clone();
    bvpl_octree_kernel_operator<T_data> oper(tree_in);
    for (unsigned int id = 0; id < kernel_vector->kernels_.size(); ++id)
    {
      temp_tree->init_cells(func_max.min_response());
      bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
      oper.operate(functor, kernel, temp_tree,level, cell_length);
      combine_kernel_responses(response_tree, temp_tree, id_tree, id);
    }
    return true;
  }

 protected:
  //: Abstract function to be implemented on children on how to combine current kernel response with output response

  virtual void combine_kernel_responses(boct_tree<short ,T_data>* tree_out, boct_tree<short ,T_data>* tree_curr,
                                        boct_tree<short ,int>* id_out, int id_curr)=0;

  //: Compares response grids and stores in out, the maximum responses. This function gets called in subclasses when implementing combine_kernel_responses

  void keep_max_response_tree(boct_tree<short ,T_data>* tree1, boct_tree<short ,T_data>* tree2,
                              boct_tree<short ,int>* id1, int id2);
};


//: Compares response grids and stores in grid1, the wining responses.
template<class T_data>
void bvpl_octree_vector_operator<T_data>::keep_max_response_tree(boct_tree<short ,T_data>* tree1, boct_tree<short ,T_data>* tree2,
                                                                 boct_tree<short ,int>* id1, int id2)
{
  bvpl_local_max_functor<T_data> func_max;

  vcl_vector<boct_tree_cell<short,T_data>*> leaves1 = tree1->leaf_cells();
  vcl_vector<boct_tree_cell<short,T_data>*> leaves2 = tree2->leaf_cells();
  vcl_vector<boct_tree_cell<short,int>*> id1_leaves = id1->leaf_cells();

  for (unsigned i=0; i< leaves1.size(); i++)
  {
    T_data val1 = leaves2[i]->data();
    T_data val2 = leaves1[i]->data();
    if (func_max.greater_than(val1, val2))
    {
      leaves1[i]->set_data(leaves2[i]->data());
      id1_leaves[i]->set_data(id2);
    }
  }
}

#endif
