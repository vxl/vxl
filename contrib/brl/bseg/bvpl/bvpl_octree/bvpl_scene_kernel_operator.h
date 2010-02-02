// This is brl/bseg/bvpl/bvpl_octree/bvpl_scene_kernel_operator.h
#ifndef bvpl_scene_kernel_operator_h
#define bvpl_scene_kernel_operator_h
//:
// \file
// \brief: A class that operates a bvpl_kernel on a boxm_scene
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 18, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/bvpl_kernel.h>
#include <boxm/boxm_scene.h>
#include "bvpl_octree_kernel_operator.h"


class bvpl_scene_kernel_operator
{
 public:
  // "Convolves" kernel with an input octree, storing the output in an output octree.
  template<class T_data, class F>
  void operate(boxm_scene<boct_tree<short, T_data > > &scene_in,
               F functor,
               bvpl_kernel_sptr kernel,
               boxm_scene<boct_tree<short, T_data > > &scene_out,
               short level)
  {
    typedef boct_tree<short, T_data > tree_type;
    boxm_block_iterator<tree_type> iter_in = scene_in.iterator();
    boxm_block_iterator<tree_type> iter_out = scene_out.iterator();;
    iter_in.begin();
    iter_out.begin();
    for (; !iter_in.end(); iter_in++, iter_out++) {
      scene_in.load_block(iter_in.index());
      scene_out.load_block(iter_out.index());
      tree_type *tree_in= (*iter_in)->get_tree();
      tree_type *tree_out = tree_in->clone();
      bvpl_octree_kernel_operator<T_data> oper(tree_in);
      double cell_length = 1.0/(double)(1<<(tree_in->root_level() -level));
      oper.operate(functor, kernel, tree_out, level, cell_length);
      (*iter_out)->init_tree(tree_out);
      scene_out.write_active_block();
    }
  }
};

#endif // bvpl_scene_kernel_operator_h
