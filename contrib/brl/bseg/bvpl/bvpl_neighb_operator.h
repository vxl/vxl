#ifndef bvpl_neighb_operator_h_
#define bvpl_neighb_operator_h_
//:
// \file
// \brief A class for performing neighborhood operations on a given voxel grid
//
// \author Gamze Tunali
// \date June 3, 2009
// \verbatim
//  Modifications
//  Gamze Tunali June 5, 2009
//      operate() method taking the grids instead of subgrid iterators, and carries out the
//      operation at every possible voxel
//
// \endverbatim

#include "bvpl_subgrid_iterator.h"
#include <bvpl/kernels/bvpl_kernel_iterator.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include "bvpl_voxel_subgrid.h"

#include <vgl/vgl_point_3d.h>

template <class T, class F>
class bvpl_neighb_operator
{
 public:
  bvpl_neighb_operator(const F functor): func_(functor) {}
  ~bvpl_neighb_operator() = default;

  // makes the neighborhood operation and stores the result in the output subgrid, which may be equal to the input
  void operate(bvxm_voxel_grid<T>* grid, bvpl_kernel_sptr kernel, bvxm_voxel_grid<T>* out_grid)
  {
    bvpl_subgrid_iterator<T> subgrid_iter(grid, kernel->min_point(), kernel->max_point());
    bvpl_subgrid_iterator<T> output_iter(out_grid, kernel->min_point(), kernel->max_point());
    //kernel->print();
    while (!subgrid_iter.isDone()) {
       bvpl_kernel_iterator kernel_iter = kernel->iterator();
       bvpl_voxel_subgrid<T> subgrid = *subgrid_iter;
       //reset the iterator
       kernel_iter.begin();
       while (!kernel_iter.isDone()) {
         vgl_point_3d<int> idx = kernel_iter.index();
         T val;
         if (subgrid.voxel(idx, val)) {
           //std::cout<< val << "at " << idx <<std::endl;
           bvpl_kernel_dispatch d = *kernel_iter;
           func_.apply(val, d);
          }
         ++kernel_iter;
       }
       // set the result at the output grid
       (*output_iter).set_voxel(func_.result());
       ++subgrid_iter;
       ++output_iter;
    }
  }

 private:
  F func_;
};

#endif
