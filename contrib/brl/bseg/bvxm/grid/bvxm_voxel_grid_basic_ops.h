// This is /contrib/brl/bseg/bvxm/grid/bvxm_voxel_grid_basic_ops.h
#ifndef bvxm_voxel_grid_basic_ops_h
#define bvxm_voxel_grid_basic_ops_h

//:
// \file
// \brief set of basic grid operations
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  6/18/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvxm_voxel_grid_base.h"
#include "bvxm_voxel_grid.h"

//: Multiplies 2 grids. The types of input grids must have a * operator
template<class T>
bool bvxm_voxel_grid_multiply(bvxm_voxel_grid_base_sptr grid1_base, bvxm_voxel_grid_base_sptr grid2_base, bvxm_voxel_grid_base_sptr grid_out_base)
{      
  //cast
  bvxm_voxel_grid<T> *grid1 = dynamic_cast<bvxm_voxel_grid<T>* >(grid1_base.ptr());
  bvxm_voxel_grid<T> *grid2 = dynamic_cast<bvxm_voxel_grid<T>* >(grid2_base.ptr());
  bvxm_voxel_grid<T> *grid_out = dynamic_cast<bvxm_voxel_grid<T>* >(grid_out_base.ptr());
  
  //check the casting was successful
  if( !grid1 || !grid2 || !grid_out)
  {
    vcl_cerr << "One of the input voxels is of the wrong type \n";
    return false;
  }
  
  //check sizes are the same
  if( grid1->grid_size() != grid2->grid_size() ||  grid1->grid_size() != grid_out->grid_size() )
  {
    vcl_cerr<< "Grids are not of the same type \n";
    return false;
  }
  
  //multipy
  typename bvxm_voxel_grid<T>::iterator grid1_it = grid1->begin();
  typename bvxm_voxel_grid<T>::iterator grid2_it = grid2->begin();
  typename bvxm_voxel_grid<T>::iterator grid_out_it = grid_out->begin();
  
  for(; grid1_it != grid1->end(); ++grid1_it, ++grid2_it, ++grid_out_it)
  {
    
    typename bvxm_voxel_slab<T>::iterator slab1_it = (*grid1_it).begin();
    typename bvxm_voxel_slab<T>::iterator slab2_it = (*grid2_it).begin();
    typename bvxm_voxel_slab<T>::iterator slab_out_it = (*grid_out_it).begin();
    
    for(; slab1_it!=(*grid1_it).end(); ++slab1_it ,++slab2_it, ++slab_out_it)
    {
      (*slab_out_it) =(*slab1_it)*(*slab2_it);
    } 
    
  }
  return true;
}

//: Thresholds a grid. This function returns the thresholded grid and a mask shuch that output grid = mask*input_grid
template<class T> 
bool bvxm_voxel_grid_threshold(bvxm_voxel_grid_base_sptr grid_in_base,bvxm_voxel_grid_base_sptr grid_out_base,
                                         bvxm_voxel_grid_base_sptr mask_grid_base, T min_thresh)
{
  //cast
  bvxm_voxel_grid<T> *grid_in = dynamic_cast<bvxm_voxel_grid<T> * > (grid_in_base.ptr());
  bvxm_voxel_grid<T> *grid_out = dynamic_cast<bvxm_voxel_grid<T> * > (grid_out_base.ptr());
  bvxm_voxel_grid<bool> *mask_grid = dynamic_cast<bvxm_voxel_grid<bool> * > (mask_grid_base.ptr());
  
  //check the casting was successful
  if( !grid_in || !grid_out || !mask_grid)
  {
    vcl_cerr << "One of the input voxels is of the wrong type \n";
    return false;
  }
  
  //check sizes are the same
  if( grid_in->grid_size() != grid_out->grid_size() ||  grid_in->grid_size() != mask_grid->grid_size() )
  {
    vcl_cerr<< "Grids are not of the same type \n";
    return false;
  }
  
  
  //intitialize grids
  grid_out->initialize_data(T(0));
  mask_grid->initialize_data(false);
  
  
  // ierate though the grids
  typename bvxm_voxel_grid<T>::iterator in_slab_it = grid_in->begin();
  typename bvxm_voxel_grid<T>::iterator out_slab_it = grid_out->begin();
  bvxm_voxel_grid<bool>::iterator mask_slab_it = mask_grid->begin();
  
  vcl_cout << "Thresholding Grid: " << vcl_endl;
  for (unsigned z=0; z<(unsigned)(grid_in->grid_size().z()); ++z, ++in_slab_it, ++mask_slab_it, ++out_slab_it)
  {
    vcl_cout << '.';
    
    //iterate through slab and threshold. At this point the grids get updated on disk
    typename bvxm_voxel_slab<T>::iterator in_it = (*in_slab_it).begin();
    typename bvxm_voxel_slab<T>::iterator out_it = (*out_slab_it).begin();
    bvxm_voxel_slab<bool>::iterator mask_it = (*mask_slab_it).begin();
    
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
