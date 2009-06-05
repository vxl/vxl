#ifndef bvpl_neighb_operator_h_
#define bvpl_neighb_operator_h_

//:
// \file
// \brief A class for performing a neighborhood operation on a given voxel world 
//
// \author Gamze Tunali
// \date June 3, 2009
// \verbatim
//  Modifications
//   
// \endverbatim

#include "bvpl_subgrid_iterator.h"
#include "bvpl_kernel_iterator.h"
#include "bvpl_voxel_subgrid.h"

#include <vgl/vgl_point_3d.h>

template <class T, class F>
class bvpl_neighb_operator
{
public:
  bvpl_neighb_operator(const F functor): func_(functor) {}
  ~bvpl_neighb_operator() {}

  // makes the neighborhood operation and stores the result in the output subgrid, which may be equal to the input
  void operate(bvpl_subgrid_iterator<T>& subgrid_iter, bvpl_kernel_iterator& kernel_iter, bvpl_subgrid_iterator<T>& output_iter)
  {
    subgrid_iter.begin();
    while (!subgrid_iter.isDone()) {
      bvpl_voxel_subgrid<T> grid = *subgrid_iter;
      //reset the iterator
      kernel_iter.begin();
      
      while (!kernel_iter.isDone()) {
        vgl_point_3d<int> idx = kernel_iter.index();
        T val;
        if (grid.voxel(idx, val)) {
          bvpl_kernel_dispatch d = *kernel_iter;
          func_.apply(val, d);
        }
        ++kernel_iter;
      }
      
      (*output_iter).set_voxel(func_.result());
      ++subgrid_iter;
    }
    
  }

private:
  F func_;
};

#endif