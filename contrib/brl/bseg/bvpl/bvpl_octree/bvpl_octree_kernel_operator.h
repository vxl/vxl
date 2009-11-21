// This is brl/bseg/bvpl/bvpl_octree/bvpl_octree_kernel_operator.h
#ifndef bvpl_octree_kernel_operator_h
#define bvpl_octree_kernel_operator_h
//:
// \file
// \brief A class that operates a bvpl_kernel on a octree
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 13, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/bvpl_kernel.h>
#include <boct/boct_tree.h>


template<class T, class F>
class bvpl_octree_kernel_operator
{
 public:

  //Constructor from a bvpl functor
  bvpl_octree_kernel_operator(const F functor): func_(functor) {}

  // "Convolves" kernel with and input octree, storing the output in an output octree.
  void operate(boct_tree<short ,T>* tree_in, bvpl_kernel_sptr kernel,  boct_tree<short,T>* out_tree, short level, double cell_length)
  {
    //get all cells at given scale
    vcl_vector<boct_tree_cell<short,T>* > cells = tree_in->cells_at_level(level);
    vcl_vector<boct_tree_cell<short,T>* > out_cells = out_tree->cells_at_level(level);

    bvpl_kernel_iterator kernel_iter = kernel->iterator();

    //iterate, through all cells
    for (unsigned i = 0; i <  cells.size();i++)
    {
      //iterate through vector an compute result
      boct_tree_cell<short,T>* center_cell = cells[i];
      vgl_point_3d<double> this_cell_origin = tree_in->local_origin(center_cell);

      // -- TO DO: check bounds.
      // Is it more efficient to check bounds for every kernel
      // or to let it run and store a dummy value in it?

      //reset the kernel iterator
      kernel_iter.begin();
      while (!kernel_iter.isDone())
      {
        vgl_point_3d<int> kernel_idx = kernel_iter.index();


        vgl_point_3d<double> this_cell_idx(this_cell_origin.x() + (double)kernel_idx.x()*cell_length,
                                           this_cell_origin.y() + (double)kernel_idx.y()*cell_length,
                                           this_cell_origin.z() + (double)kernel_idx.z()*cell_length);

        boct_tree_cell<short,T>* this_cell = tree_in->locate_point_at_level(this_cell_idx, level, true);

        if (this_cell) {
          bvpl_kernel_dispatch d = *kernel_iter;
          T val = this_cell->data();
          func_.apply(val, d);
        }
        else
          break;
        ++kernel_iter;
      }

      out_cells[i]->set_data(func_.result());
    }
  }

 private:
  F func_;
};

#endif
