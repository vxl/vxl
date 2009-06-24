// This is brl/bseg/bvpl/bvpl_vector_operator.h
#ifndef bvpl_vector_operator_h
#define bvpl_vector_operator_h

//:
// \file
// \brief A class to apply a vector of kernels to a voxel grid
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  6/24/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_neighb_operator.h"
#include "bvpl_kernel_factory.h"

#include <vcl_limits.h>

template<class T, class F>
class bvpl_vector_operator
{
public:
  
  //: Applies a vector of kernel and functor to a grid. The result is  a grid of maxima response and  a grid 
  //  containing a vnl_vectors in the orientation of maxima response e.i. the direction is rotation axis, the magnitude is the angle
  void max_orientation(bvxm_voxel_grid<T>* grid, bvpl_kernel_vector_sptr kernel_vector,
                       bvpl_neighb_operator<T,F>* oper, bvxm_voxel_grid<T>* out_grid,  
                       bvxm_voxel_grid<T>* orientation_grid);

  void get_max_orientation_grid(bvxm_voxel_grid<T>* out_grid, bvxm_voxel_grid<T>* temp_grid,
                                bvxm_voxel_grid<T>* orientation_grid, 
                                vnl_vector_fixed<float,3> temp_orientation);
private:

};

template<class T, class F>
void bvpl_vector_operator<T,F>::max_orientation(bvxm_voxel_grid<T>* grid, bvpl_kernel_vector_sptr kernel_vector,
                                               bvpl_neighb_operator<T,F>* oper, bvxm_voxel_grid<T>* out_grid,  
                                               bvxm_voxel_grid<T>* orientation_grid)
{
  bvxm_voxel_grid<T> temp_grid("temp_grid.vox",out_grid->grid_size());
  bvxm_voxel_grid<T> temp_orientation_grid("temp_grid.vox",out_grid->grid_size());
  out_grid->initialize_data(vcl_numeric_limits<T>::min);
  temp_grid.initialize_data(T(0.0));
  orientation_grid->initialize_data(vnl_vector_fixed<float,3>(0.0f,0.0f,0.0f));
  bvpl_kernel_vector::iterator vit = kernel_vector->kernels_.begin();
  vnl_vector_fixed<float,3> curr_axis( 0.0f, 0.0f, 0.0f);
  
  for(; vit!=kernel_vector->kernels_.end();		vit++)
  {
    bvpl_kernel_sptr kernel = (*vit).second;
    vnl_vector_fixed<float, 3> axis = vit->first;
    oper->operate(grid, kernel, &temp_grid);
    get_max_orientation_grid(out_grid, &temp_grid, orientation_grid, axis);
  }
  
}

template<class T, class F>
void bvpl_vector_operator<T,F>::get_max_orientation_grid(bvxm_voxel_grid<T>* out_grid, bvxm_voxel_grid<T>* temp_grid,
                                                    bvxm_voxel_grid<T>* orientation_grid, vnl_vector_fixed<float,3> temp_orientation)
{
  typename bvxm_voxel_grid<T>::iterator out_grid_it = out_grid->begin();
  typename bvxm_voxel_grid<T>::iterator temp_grid_it = temp_grid->begin();
  bvxm_voxel_grid<vnl_vector_fixed<float,3> >::iterator or_grid_it = orientation_grid->begin();
 
  
  for (; out_grid_it!=out_grid->end(); ++out_grid_it, ++temp_grid_it, ++or_grid_it) 
  {
    
    typename bvxm_voxel_slab<T>::iterator out_slab_it = out_grid_it->begin();
    typename bvxm_voxel_grid<T>::iterator temp_slab_it= temp_grid_it->begin();
    bvxm_voxel_grid<vnl_vector_fixed<float,3> >::iterator or_slab_it = or_grid_it->begin();

    
    for (; out_slab_it!=out_grid_it->end(); ++out_slab_it, ++temp_slab_it, ++or_slab_it) 
    {
      if( (* temp_slab_it) > (*out_slab_it) )
      {
        (*out_slab_it) =  (* temp_slab_it);
        (*or_slab_it) = temp_orientation;
      }
    }
  }
}
#endif
