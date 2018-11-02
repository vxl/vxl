// This is brl/bseg/bvpl/bvpl_vector_operator.h
#ifndef bvpl_vector_operator_h
#define bvpl_vector_operator_h
//:
// \file
// \brief A class to apply a vector of kernels to a voxel grid
// \author Isabel Restrepo mir@lems.brown.edu
// \date  June 24, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <limits>
#include "bvpl_neighb_operator.h"
#include "bvpl_combined_neighb_operator.h"
#include <bvpl/kernels/bvpl_kernel_factory.h>
#include <bvpl/functors/bvpl_local_max_functor.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>

class bvpl_vector_operator
{
 public:
  //: Applies a vector of kernel and functor to a grid.
  //  The result is a grid of maxima response and a grid and id to the winning kernel
  template<class T, class F>
  void apply_and_suppress(bvxm_voxel_grid<T>* grid, bvpl_kernel_vector_sptr kernel_vector,
                          bvpl_neighb_operator<T,F>* oper, bvxm_voxel_grid<T>* out_grid,
                          bvxm_voxel_grid<int >* id_grid);

  //: Non-max suppression among all features, the result is stored in the input grid, as previous results are necessary
  template<class T>
  void non_maxima_suppression(bvxm_voxel_grid<T>* grid_in, bvxm_voxel_grid<int >* id_grid,
                              bvpl_kernel_vector_sptr kernel_vectors);

  //: Non-max suppression among features of its own kind
  template<class T>
  void local_non_maxima_suppression(bvxm_voxel_grid<T>* grid_in, bvxm_voxel_grid<int >* id_grid,
                                    bvpl_kernel_vector_sptr kernel_vector);

  //: Returns the response corresponding to the target id, all others are set to zero.
  template<class T>
  void filter_response(bvxm_voxel_grid<T>* grid_in, bvxm_voxel_grid<int >* id_grid,
                       int targed_id, bvxm_voxel_grid<float>* out_grid);

  //: Returns a grid containing single responses from a grid containing vectors of responses
  template<unsigned n_responses>
  void get_response(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* resp_grid,
                    bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
                    unsigned idx,
                    bvxm_voxel_grid<float>* out_grid, bvxm_voxel_grid<int>* out_id_grid);

  template<class T>
  void get_max_orientation_grid(bvxm_voxel_grid<T>* out_grid, bvxm_voxel_grid<T>* temp_grid,
                                bvxm_voxel_grid<int >* id_grid,
                                int temp_orientation);
};


//: local non-max suppression, the result is stored in the input grid, as previous results are necessary
template<class T>
void bvpl_vector_operator::non_maxima_suppression(bvxm_voxel_grid<T>* grid_in,
                                                  bvxm_voxel_grid<int >* id_grid,
                                                  bvpl_kernel_vector_sptr kernel_vector)
{
  bvpl_subgrid_iterator<T> response_grid_iter(grid_in, kernel_vector->min(), kernel_vector->max());
   bvpl_subgrid_iterator<int> kernel_id_iter(id_grid, kernel_vector->min(), kernel_vector->max());
  bvpl_local_max_functor<T> func_max;
  //kernel->print();
  while (!response_grid_iter.isDone()) {
    int index=(*kernel_id_iter).get_voxel();
    bvpl_kernel_iterator kernel_iter = kernel_vector->kernels_[index]->iterator();
    bvpl_voxel_subgrid<T> subgrid = *response_grid_iter;
    //reset the iterator
    kernel_iter.begin();
    while (!kernel_iter.isDone()) {
      vgl_point_3d<int> idx = kernel_iter.index();
      T val;
     //we don't want to apply to the current center voxels
      if (!((idx.x() == 0) && (idx.y() == 0) && (idx.z() == 0)))
        if (subgrid.voxel(idx, val)) {
#if 0
          std::cout << "val" << val << "at " << idx <<std::endl;
#endif
          bvpl_kernel_dispatch d = *kernel_iter;
          func_max.apply(val, d);
        }
      ++kernel_iter;
    }

    // set the result at the input grid
    subgrid.set_voxel(func_max.result(subgrid.get_voxel()));
    ++response_grid_iter;
    ++kernel_id_iter;
  }
}


// Non maxima suppression for features of the same kind
template<class T>
void bvpl_vector_operator::local_non_maxima_suppression(bvxm_voxel_grid<T>* grid_in,
                                                        bvxm_voxel_grid<int>* id_grid,
                                                        bvpl_kernel_vector_sptr kernel_vector)
{
  bvpl_subgrid_iterator<T> response_grid_iter(grid_in, kernel_vector->min(), kernel_vector->max());
  //bvpl_subgrid_iterator<T> out_grid_iter(grid_out, kernel_vector->max_dim());
  bvpl_subgrid_iterator<int> kernel_id_iter(id_grid, kernel_vector->min(), kernel_vector->max());

  bvpl_local_max_functor<T> func_max;
  //kernel->print();
  while (!response_grid_iter.isDone()) {
    bvpl_voxel_subgrid<T> subgrid = *response_grid_iter;
    bvpl_voxel_subgrid<int> id_subgrid = *kernel_id_iter;
    int index=id_subgrid.get_voxel();
    bvpl_kernel_iterator kernel_iter = kernel_vector->kernels_[index]->iterator();
    //reset the iterator
    kernel_iter.begin();
    while (!kernel_iter.isDone()) {
      vgl_point_3d<int> idx = kernel_iter.index();
      T val;
      int id_val;
      //we don't want to apply to the current center voxels
      if (!((idx.x() == 0) && (idx.y() == 0) && (idx.z() == 0)))
        if (subgrid.voxel(idx, val) && id_subgrid.voxel(idx, id_val)) {
          if (id_val == index){
#if 0
            std::cout << "val" << val << "at " << idx <<std::endl;
#endif
            bvpl_kernel_dispatch d = *kernel_iter;
            func_max.apply(val, d);
          }
        }
      ++kernel_iter;
    }

    // set the result at the input grid
    subgrid.set_voxel(func_max.result(subgrid.get_voxel()));
    ++response_grid_iter;
    ++kernel_id_iter;
  }
}


template<class T, class F>
void bvpl_vector_operator::apply_and_suppress(bvxm_voxel_grid<T>* grid,
                                              bvpl_kernel_vector_sptr kernel_vector,
                                              bvpl_neighb_operator<T,F>* oper,
                                              bvxm_voxel_grid<T>* out_grid,
                                              bvxm_voxel_grid<int>* id_grid)
{
  bvpl_local_max_functor<T> func_max;
  bvxm_voxel_grid<T> temp_grid("temp_grid.vox",grid->grid_size());
  temp_grid.initialize_data(func_max.min_response());
  out_grid->initialize_data(func_max.min_response());
  id_grid->initialize_data(-1);

  for (unsigned int id = 0; id < kernel_vector->kernels_.size(); ++id)
  {
    bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
#if 0
    std::cout << "Processing axis: "  << kernel->axis() << std::endl
             << "Processing angle: " << kernel->angle() << std::endl;
             << "Processing scale: " << kernel->scale() << std::endl;
#endif
    oper->operate(grid, kernel, &temp_grid);
    get_max_orientation_grid(out_grid, &temp_grid, id_grid, id);
  }

  vpl_unlink("temp_grid.vox");
}

template<class T>
void  bvpl_vector_operator::filter_response(bvxm_voxel_grid<T>* grid_in, bvxm_voxel_grid<int >* id_grid,
                                            int targed_id, bvxm_voxel_grid<float>* out_grid)
{
  bvpl_local_max_functor<T> func_max;
  out_grid->initialize_data(0.0f);
  bvxm_voxel_grid<float>::iterator out_grid_it = out_grid->begin();
  typename bvxm_voxel_grid<T>::iterator in_grid_it = grid_in->begin();
  bvxm_voxel_grid<int >::iterator id_grid_it = id_grid->begin();

  for (; out_grid_it!=out_grid->end(); ++out_grid_it, ++in_grid_it, ++id_grid_it)
  {
    bvxm_voxel_slab<float>::iterator out_slab_it = (*out_grid_it).begin();
    typename bvxm_voxel_slab<T>::iterator in_slab_it= (*in_grid_it).begin();
    bvxm_voxel_slab<int>::iterator id_slab_it = id_grid_it->begin();

    for (; out_slab_it!=(*out_grid_it).end(); ++out_slab_it, ++in_slab_it, ++id_slab_it)
    {
      *out_slab_it = func_max.filter_response(*id_slab_it, targed_id, *in_slab_it);
    }
  }
}


//: Returns a grid containing single responses from a grid containing vectors of responses
template<unsigned n_responses>
void bvpl_vector_operator::get_response(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* resp_grid,
                                        bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
                                        unsigned idx,
                                        bvxm_voxel_grid<float>* out_grid, bvxm_voxel_grid<int>* out_id_grid)
{
  bvxm_voxel_grid<float>::iterator out_grid_it = out_grid->begin();
  bvxm_voxel_grid<int >::iterator out_id_grid_it = out_id_grid->begin();
  typename bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >::iterator resp_grid_it = resp_grid->begin();
  typename bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >::iterator id_grid_it = id_grid->begin();

  for (; out_grid_it!=out_grid->end(); ++out_grid_it, ++out_id_grid_it, ++resp_grid_it, ++id_grid_it)
  {
    bvxm_voxel_slab<float>::iterator out_slab_it = out_grid_it->begin();
    bvxm_voxel_slab<int >::iterator out_id_slab_it = out_id_grid_it->begin();
    typename bvxm_voxel_slab<vnl_vector_fixed<float, n_responses> >::iterator resp_slab_it = resp_grid_it->begin();
    typename bvxm_voxel_slab<vnl_vector_fixed<int, n_responses> >::iterator id_slab_it = id_grid_it->begin();

    for (; out_slab_it!=(*out_grid_it).end(); ++out_slab_it, ++out_id_slab_it, ++resp_slab_it,++id_slab_it)
    {
      *out_slab_it = (*resp_slab_it)[idx];
      *out_id_slab_it = (*id_slab_it)[idx];
    }
  }
}


template<class T>
void bvpl_vector_operator::get_max_orientation_grid(bvxm_voxel_grid<T>* out_grid, bvxm_voxel_grid<T>* temp_grid,
                                                    bvxm_voxel_grid<int>* id_grid,
                                                    int id)
{
  bvpl_local_max_functor<T> func_max;
  typename bvxm_voxel_grid<T>::iterator out_grid_it = out_grid->begin();
  typename bvxm_voxel_grid<T>::iterator temp_grid_it = temp_grid->begin();
  bvxm_voxel_grid<int >::iterator id_grid_it = id_grid->begin();

  for (; out_grid_it!=out_grid->end(); ++out_grid_it, ++temp_grid_it, ++id_grid_it)
  {
    typename bvxm_voxel_slab<T>::iterator out_slab_it = (*out_grid_it).begin();
    typename bvxm_voxel_slab<T>::iterator temp_slab_it= (*temp_grid_it).begin();
    bvxm_voxel_slab<int>::iterator id_slab_it = id_grid_it->begin();

    for (; out_slab_it!=(*out_grid_it).end(); ++out_slab_it, ++temp_slab_it, ++id_slab_it)
    {
      if (func_max.greater_than((* temp_slab_it), (*out_slab_it)))
      {
        (*out_slab_it) =  (* temp_slab_it);
        (*id_slab_it) = id;
      }
    }
  }
}

#endif // bvpl_vector_operator_h
