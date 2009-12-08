// This is brl/bseg/bvpl/bvpl_octree/bvpl_octree_kernel_operator.h
#ifndef bvpl_octree_kernel_operator_h
#define bvpl_octree_kernel_operator_h
//:
// \file
// \brief A class that operates a bvpl_kernel on a octree or boxm scene
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 13, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/bvpl_kernel.h>
#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>


template<class T_data>
class bvpl_octree_kernel_operator
{
 public:
  typedef boct_tree<short, T_data> tree_type;
  typedef boct_tree_cell<short,T_data> cell_type;

  //Constructor from a bvpl functor
  bvpl_octree_kernel_operator(tree_type* tree_in): tree_in_(tree_in){}

  // "Convolves" kernel with class tree, storing the output in an output octree.
  template<class F>
  void operate(F functor, bvpl_kernel_sptr kernel, tree_type* out_tree, short level, double cell_length)
  {
    //get all cells at given scale
    vcl_vector<cell_type* > cells = tree_in_->cells_at_level(level);
    vcl_vector<cell_type* > out_cells = out_tree->cells_at_level(level);

    bvpl_kernel_iterator kernel_iter = kernel->iterator();

    //iterate, through all cells
    for (unsigned i = 0; i <  cells.size();i++)
    {
      // iterate through vector an compute result
      cell_type* center_cell = cells[i];
      vgl_point_3d<double> this_cell_origin = tree_in_->local_origin(center_cell);

      // TODO: check bounds. Is it more efficient to check bounds for every kernel or to let is run and store a dummy value in it?

      kernel_iter.begin(); // reset the kernel iterator
      while (!kernel_iter.isDone())
      {
        vgl_point_3d<int> kernel_idx = kernel_iter.index();

        vgl_point_3d<double> this_cell_idx(this_cell_origin.x() + (double)kernel_idx.x()*cell_length,
                                           this_cell_origin.y() + (double)kernel_idx.y()*cell_length,
                                           this_cell_origin.z() + (double)kernel_idx.z()*cell_length);

        cell_type* this_cell = tree_in_->locate_point_at_level(this_cell_idx, level, true);

        if (this_cell) {
          bvpl_kernel_dispatch d = *kernel_iter;
          T_data val = this_cell->data();
          functor.apply(val, d);
        }
        else
          break;
        ++kernel_iter;
      }

      out_cells[i]->set_data(functor.result());
    }
  }

 private:
  tree_type * tree_in_;
};

#endif // bvpl_octree_kernel_operator_h
