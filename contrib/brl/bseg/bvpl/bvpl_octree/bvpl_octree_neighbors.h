// This is brl/bseg/bvpl/bvpl_octree/bvpl_octree_neighbors.h
#ifndef bvpl_octree_neighbors_h
#define bvpl_octree_neighbors_h
//:
// \file
// \brief  A class that operates a bvpl_kernel on an octree or boxm scene to find all the neighbor cells that correspond to that kernel, and returns a list of pointers to those neighbors
// \author Gamze Tunali Gamze_Tunali@brown.edu
// \date   March 18, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bvpl/kernels/bvpl_kernel.h>
#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<class T_data>
class bvpl_octree_neighbors
{
 public:
  typedef boct_tree<short, T_data> tree_type;
  typedef boct_tree_cell<short,T_data> cell_type;

  //Constructor from a bvpl functor
  bvpl_octree_neighbors(tree_type* tree_in): tree_in_(tree_in){}

  // "Convolves" kernel with class tree, storing the output in an output octree.
  void neighbors(bvpl_kernel_sptr kernel, cell_type* cell,
                 std::vector<cell_type *>& neighb_cells)
  {
    bvpl_kernel_iterator kernel_iter = kernel->iterator();
    short level = cell->level();
    double cell_length = tree_in_->cell_length(cell);
    // iterate through vector and find the neighbors
    vgl_point_3d<double> cell_origin = tree_in_->local_origin(cell);

    kernel_iter.begin(); // reset the kernel iterator
    while (!kernel_iter.isDone())
    {
      vgl_point_3d<int> kernel_idx = kernel_iter.index();
      vgl_point_3d<double> this_cell_idx(cell_origin.x() + (double)kernel_idx.x()*cell_length,
                                         cell_origin.y() + (double)kernel_idx.y()*cell_length,
                                         cell_origin.z() + (double)kernel_idx.z()*cell_length);

      cell_type* this_cell = tree_in_->locate_point_at_level(this_cell_idx, level, true);
      if (this_cell) {
        // check if this cell is an internal node, if so get the children
        if (!this_cell->is_leaf()) {
          std::vector<cell_type *> children;
          this_cell->leaf_children(children);
          neighb_cells.insert(neighb_cells.end(), children.begin(), children.end());
        }
        else {
          // make sure that the same cell is not added more than once
          bool found=false;
          for (unsigned i=0; i<neighb_cells.size(); i++) {
            if (neighb_cells[i] == this_cell)
              found=true;
          }
          if (!found)
            neighb_cells.push_back(this_cell);
        }
      }
      ++kernel_iter;
    }
  }

 private:
  tree_type * tree_in_;
};

#endif // bvpl_octree_neighbors_h
