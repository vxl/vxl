// This is brl/bseg/bvxm/grid/bvxm_voxel_grid_basic_ops.h
#ifndef bvxm_voxel_grid_basic_ops_h
#define bvxm_voxel_grid_basic_ops_h
//:
// \file
// \brief set of basic grid operations
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  June 18, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvxm_voxel_grid.h"
#include <vcl_iostream.h>

//: Multiplies 2 grids. The types of input grids must have a * operator
template<class T>
bool multiply(bvxm_voxel_grid<T> const& grid1, bvxm_voxel_grid<T> const& grid2, bvxm_voxel_grid<T> & grid_out)
{
  //multipy
  typename bvxm_voxel_grid<T>::iterator grid1_it = grid1.begin();
  typename bvxm_voxel_grid<T>::iterator grid2_it = grid2.begin();
  typename bvxm_voxel_grid<T>::iterator grid_out_it = grid_out.begin();

  for (; grid1_it != grid1.end(); ++grid1_it, ++grid2_it, ++grid_out_it)
  {
    typename bvxm_voxel_slab<T>::iterator slab1_it = (*grid1_it).begin();
    typename bvxm_voxel_slab<T>::iterator slab2_it = (*grid2_it).begin();
    typename bvxm_voxel_slab<T>::iterator slab_out_it = (*grid_out_it).begin();

    for (; slab1_it!=(*grid1_it).end(); ++slab1_it ,++slab2_it, ++slab_out_it)
    {
      (*slab_out_it) =(*slab1_it)*(*slab2_it);
    }
  }
  return true;
}

//: Thresholds a grid. This function returns the thresholded grid and a mask shuc that output grid = mask*input_grid
template<class T>
bool threshold(bvxm_voxel_grid<T> const& grid_in,bvxm_voxel_grid<T> const grid_out,
               bvxm_voxel_grid<bool> const mask, float min_thresh)
{
  //initialize grids
  grid_out->initialize_data(T(0));
  mask_grid->initialize_data(T(0));

  // ierate though the grids
  typename bvxm_voxel_grid<T>::iterator in_slab_it = grid_in->begin();
  typename bvxm_voxel_grid<T>::iterator out_slab_it = grid_out->begin();
  typename bvxm_voxel_grid<bool::iterator mask_slab_it = mask_grid->begin();

  vcl_cout << "Thresholding Grid: " << vcl_endl;
  for (unsigned z=0; z<(unsigned)(grid_in->grid_size().z()); ++z, ++in_slab_it, ++mask_slab_it, ++out_slab_it)
  {
    vcl_cout << '.';

    //iterate through slab and threshold. At this point the grids get updated on disk
    typename bvxm_voxel_slab<T>::iterator in_it = (*in_slab_it).begin();
    typename bvxm_voxel_slab<T>::iterator out_it = (*out_slab_it).begin();
    typename bvxm_voxel_slab<bool>::iterator mask_it = (*mask_slab_it).begin();

    for (; in_it!= (*in_slab_it).end(); ++in_it, ++out_it, ++mask_it)
    {
      if (*in_it > min_thresh){
        (*mask_it) = true;
        //if point is above threshold leave unchanged, otherwise set to 0
        (*out_it) = (*in_it);
      }
    }
  }
  return true;
}


#endif
