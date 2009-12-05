// This is bvpl_scene_vector_operator.h
#ifndef bvpl_scene_vector_operator_h
#define bvpl_scene_vector_operator_h

//:
// \file
// \brief  A class that operates a vector of bvpl_kernels on a boxm_scene
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
    

  //: Main interface funtion. Applies a vector of bvpl_kernels on a boxm_scene
  template<class T_data, class F >
  bool operate(boxm_scene<boct_tree<short, T_data> > &scene_in, 
               bvpl_octree_vector_operator<T_data> * vec_opertor,
               F functor,
               bvpl_kernel_vector_sptr kernel_vector,
               boxm_scene<boct_tree<short, T_data> > &scene_out,
               boxm_scene<boct_tree<short ,int> >& id_scene, short level, double cell_length)
  {
    //iterate through the scene
    boxm_block_iterator<boct_tree<short, T_data> > iter_in = scene_in.iterator();
    boxm_block_iterator<boct_tree<short, T_data> > iter_out = scene_out.iterator();
    boxm_block_iterator<boct_tree<short, int> > iter_id = id_scene.iterator();
    iter_in.begin();
    iter_out.begin();
    iter_id.begin();
    
    bvpl_local_max_functor<T_data> func_max;

    for (; !iter_in.end(); iter_in++, iter_out++) {
      scene_in.load_block(iter_in.index());
      scene_out.load_block(iter_out.index());
      boct_tree<short, T_data>  *tree_in= (*iter_in)->get_tree();
      boct_tree<short, T_data>  *tree_out = tree_in -> clone();
      tree_out->init_cells(func_max.min_response());
      boct_tree<short,int> *id_tree = tree_in->template clone_to_type<int>();
      id_tree->init_cells(-1);
           
      //at each tree, run the vector of kernels
      vec_opertor->operate(tree_in, functor, kernel_vector, tree_out, id_tree, level, cell_length);      
      (*iter_out)->init_tree(tree_out);
      scene_out.write_active_block();
      (*iter_id)->init_tree(id_tree);
      id_scene.write_active_block();
    }
 
    return true;
    
  }

};

#endif
