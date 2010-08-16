// This is brl/bseg/bvpl/bvpl_octree/bvpl_scene_vector_operator.h
#ifndef bvpl_scene_vector_operator_h
#define bvpl_scene_vector_operator_h
//:
// \file
// \brief  A class that applies a vector of bvpl_kernels on a boxm_scene
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 30, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_octree_vector_operator.h"

#include <boxm/boxm_scene.h>
#include <bvpl/bvpl_kernel.h>


class bvpl_scene_vector_operator
{
 public:
  //: Main interface function. Applies a vector of bvpl_kernels on a boxm_scene. Combines the responses on a single scene
  template<class T_data, class F >
  bool operate(boxm_scene<boct_tree<short, T_data> > &scene_in,
               bvpl_octree_vector_operator<T_data> * vec_opertor,
               F functor,
               bvpl_kernel_vector_sptr kernel_vector,
               boxm_scene<boct_tree<short, bvpl_octree_sample<T_data> > > &scene_out,
              short level=0)
  {
    // iterate through the scene
    boxm_block_iterator<boct_tree<short, T_data> > iter_in = scene_in.iterator();
    boxm_block_iterator<boct_tree<short, bvpl_octree_sample<T_data> > > iter_out = scene_out.iterator();
    iter_in.begin();
    iter_out.begin();

    bvpl_local_max_functor<T_data> func_max;

    for (; !iter_in.end(); iter_in++, iter_out++) {
      scene_in.load_block(iter_in.index());
      scene_out.load_block(iter_out.index());
      boct_tree<short, T_data>  *tree_in= (*iter_in)->get_tree();
      boct_tree<short, bvpl_octree_sample<T_data> >  *tree_out = tree_in ->template clone_to_type<bvpl_octree_sample<T_data> >();
      tree_out->init_cells(bvpl_octree_sample<T_data>());
      double cell_length = 1.0/(double)(1<<(tree_in->root_level() -level));
      //at each tree, run the vector of kernels
      vec_opertor->operate(tree_in, functor, kernel_vector, tree_out, level, cell_length);      
      (*iter_out)->init_tree(tree_out);
      scene_out.write_active_block();
    }
    return true;
  }
  
};

#endif // bvpl_scene_vector_operator_h
