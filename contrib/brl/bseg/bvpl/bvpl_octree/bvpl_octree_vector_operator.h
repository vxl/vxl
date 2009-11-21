// This is brl/bseg/bvpl/bvpl_octree/bvpl_octree_vector_operator.h
#ifndef bvpl_octree_vector_operator_h
#define bvpl_octree_vector_operator_h
//:
// \file
// \brief  A class that operates a vector of bvpl_kernel on a octree
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 17, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/bvpl_kernel.h>
#include <bvpl/bvpl_local_max_functor.h>
#include <boct/boct_tree.h>
#if 0
#include <vcl_iostream.h>
#endif

class bvpl_octree_vector_operator
{
 public:
  //: Applies a vector of kernel and functor to a grid. Returns a grid of containing at each voxel the max response
  template<class T, class F>
  void max_response(boct_tree<short ,T>* tree_in, bvpl_kernel_vector_sptr kernel_vector,
                    bvpl_octree_kernel_operator<T,F>* oper, boct_tree<short ,T>* tree_out,
                    boct_tree<short ,int>* id_tree, short level, double cell_length);

  //: Compares response grids and stores in grid1, the wining responses.
  template<class T>
  void keep_max_response_tree(boct_tree<short ,T>* tree1, boct_tree<short ,T>* tree2,
                              boct_tree<short ,int>* id1, int id2);
};


//: Applies a vector of kernel and functor to a grid. Returns a grid of containing at each voxel the max response
template<class T, class F>
void bvpl_octree_vector_operator::max_response(boct_tree<short ,T>* tree_in, bvpl_kernel_vector_sptr kernel_vector,
                                               bvpl_octree_kernel_operator<T,F>* oper, boct_tree<short ,T>* tree_out,
                                               boct_tree<short ,int>* id_tree, short level, double cell_length)
{
  bvpl_local_max_functor<T> func_max;
  boct_tree<short ,T>* temp_tree = tree_in->clone();
  temp_tree->init_cells(func_max.min_response());
  tree_out->init_cells(func_max.min_response());
  id_tree->init_cells(-1);

  for (unsigned int id = 0; id < kernel_vector->kernels_.size(); ++id)
  {
    bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
#if 0
    vcl_cout << "Processing axis: "  << kernel->axis() << vcl_endl
             << "Processing angle: " << kernel->angle() << vcl_endl;
             << "Processing scale: " << kernel->scale() << vcl_endl;
#endif
    oper->operate(tree_in, kernel, temp_tree,level, cell_length);
    keep_max_response_tree(tree_out, temp_tree, id_tree, id);
  }
}

//: Compares response grids and stores in grid1, the wining responses.
template<class T>
void bvpl_octree_vector_operator::keep_max_response_tree(boct_tree<short ,T>* tree1, boct_tree<short ,T>* tree2,
                                                         boct_tree<short ,int>* id1, int id2)
{
  bvpl_local_max_functor<T> func_max;

  vcl_vector<boct_tree_cell<short,T>*> leaves1 = tree1->leaf_cells();
  vcl_vector<boct_tree_cell<short,T>*> leaves2 = tree2->leaf_cells();
  vcl_vector<boct_tree_cell<short,int>*> id1_leaves = id1->leaf_cells();

  for (unsigned i=0; i< leaves1.size(); i++)
  {
    T val1 = leaves2[i]->data();
    T val2 = leaves1[i]->data();
    if (func_max.greater_than(val1, val2))
    {
      leaves1[i]->set_data(leaves2[i]->data());
      id1_leaves[i]->set_data(id2);
    }
  }
}

#endif
