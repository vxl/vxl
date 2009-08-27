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
#include <bvpl/bvpl_local_max_functor.h>
#include <vcl_iostream.h>
#include <vcl_limits.h>

template<class T, class F>
class bvpl_vector_operator
{
 public:

  //: Applies a vector of kernel and functor to a grid.
  //  The result is a grid of maxima response and a grid containing a vnl_vector
  //  in the orientation of maxima response
  //  i.e. the direction is the rotation axis, the magnitude is the angle
  void apply_and_suppress(bvxm_voxel_grid<T>* grid, bvpl_kernel_vector_sptr kernel_vector,
                          bvpl_neighb_operator<T,F>* oper, bvxm_voxel_grid<T>* out_grid,
                          bvxm_voxel_grid<unsigned >* orientation_grid);

  //
  void get_max_orientation_grid(bvxm_voxel_grid<T>* out_grid, bvxm_voxel_grid<T>* temp_grid,
                                bvxm_voxel_grid<unsigned >* orientation_grid,
                                unsigned temp_orientation);
private:
  bvpl_local_max_functor<T> func_max_;
};

template<class T, class F>
void bvpl_vector_operator<T,F>::apply_and_suppress(bvxm_voxel_grid<T>* grid, 
                                                   bvpl_kernel_vector_sptr kernel_vector,
                                                   bvpl_neighb_operator<T,F>* oper, 
                                                   bvxm_voxel_grid<T>* out_grid,
                                                   bvxm_voxel_grid<unsigned>* id_grid)
{
  bvxm_voxel_grid<T> temp_grid("temp_grid.vox",grid->grid_size());
  temp_grid.initialize_data(func_max_.min_response());
  out_grid->initialize_data(func_max_.min_response());
  id_grid->initialize_data(0);
  bvpl_kernel_vector::iterator vit = kernel_vector->kernels_.begin();
 
  
  //for (; vit!=kernel_vector->kernels_.end(); ++vit)
  for(unsigned id = 0; id < kernel_vector->kernels_.size(); ++id)
  {
    bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
    vcl_cout << "Processing axis: "  << kernel->axis() << vcl_endl;
    vcl_cout << "Processing angle: " << kernel->angle() << vcl_endl;
    vcl_cout << "Processing scale: " << kernel->scale() << vcl_endl;
    oper->operate(grid, kernel, &temp_grid);
    get_max_orientation_grid(out_grid, &temp_grid, id_grid, id);
  }
 
#if 0
  //: code for local max suppression
  //bvpl_subgrid_iterator<T> response_grid_iter(out_grid, (*vit).second->dim());
  //bvpl_subgrid_iterator<unsigned int> kernel_id_iter(&kernel_index_grid, (*vit).second->dim());

  //bvpl_local_max_functor<T> func_max;
  ////kernel->print();
  //while (!response_grid_iter.isDone()) {
  //  unsigned index=(*kernel_id_iter).get_voxel();
  //  //: get the kernel according to the
  //  bvpl_kernel_iterator kernel_iter = kernel_vector->kernels_[index].second->iterator();
  //  bvpl_voxel_subgrid<T> subgrid = *response_grid_iter;
  //  //reset the iterator
  //  kernel_iter.begin();
  //  while (!kernel_iter.isDone()) {
  //    vgl_point_3d<int> idx = kernel_iter.index();
  //    T val;
  //    if (subgrid.voxel(idx, val)) {
  //      //vcl_cout<< val << "at " << idx <<vcl_endl;
  //      bvpl_kernel_dispatch d = *kernel_iter;
  //      func_max.apply(val, d);
  //    }
  //    ++kernel_iter;
  //  }

  //  // set the result at the output grid
  //  (*response_grid_iter).set_voxel(func_max.result());
  //  ++response_grid_iter;
  //  ++kernel_id_iter;
  //}
#endif
}

template<class T, class F>
void bvpl_vector_operator<T,F>::get_max_orientation_grid(bvxm_voxel_grid<T>* out_grid, bvxm_voxel_grid<T>* temp_grid,
                            bvxm_voxel_grid<unsigned>* id_grid,
                            unsigned id)
{
  typename bvxm_voxel_grid<T>::iterator out_grid_it = out_grid->begin();
  typename bvxm_voxel_grid<T>::iterator temp_grid_it = temp_grid->begin();
  bvxm_voxel_grid<unsigned >::iterator id_grid_it = id_grid->begin();
  
  for (; out_grid_it!=out_grid->end(); ++out_grid_it, ++temp_grid_it, ++id_grid_it)
  {
    typename bvxm_voxel_slab<T>::iterator out_slab_it = (*out_grid_it).begin();
    typename bvxm_voxel_slab<T>::iterator temp_slab_it= (*temp_grid_it).begin();
    bvxm_voxel_slab<unsigned>::iterator id_slab_it = id_grid_it->begin();

    for (; out_slab_it!=(*out_grid_it).end(); ++out_slab_it, ++temp_slab_it, ++id_slab_it)
    {
      if(func_max_.greater_than((* temp_slab_it), (*out_slab_it)))
      {
        (*out_slab_it) =  (* temp_slab_it);
        (*id_slab_it) = id;
      }
    }
  }
}

#endif // bvpl_vector_operator_h
