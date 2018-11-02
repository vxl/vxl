// This is brl/bseg/bvpl/bvpl_discriminative_non_max_suppression.h
#ifndef bvpl_discriminative_non_max_suppression_h
#define bvpl_discriminative_non_max_suppression_h
//:
// \file
// \brief  Methods to perform non-maxima suppression only on features of its own kind
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
#include <vul/vul_file.h>

class bvpl_discriminative_non_max_suppression
{
 public:

  //: Applies a one kernel at a time, performs non-maxima suppression and combines the top three responses at every  voxel
  template<class T, class F>
  void suppress_and_combine(bvxm_voxel_grid<T>* grid_in,  bvpl_kernel_vector_sptr kernel_vector,
                            bvpl_neighb_operator<T,F>* oper, bvxm_voxel_grid<vnl_vector_fixed<float,3> >* out_grid,
                            bvxm_voxel_grid<vnl_vector_fixed<int,3> >* id_grid);

  //: Using appearance and occupancy grids simultaneously, this function applies a one kernel at a time, performs non-maxima suppression and combines the top three responses at every voxel
  template<class OCP_FUNC, class APP_FUNC>
  void suppress_and_combine(bvxm_voxel_grid<float>* ocp_grid, bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >* app_grid, bvpl_kernel_vector_sptr kernel_vector,
                            bvpl_combined_neighb_operator<OCP_FUNC,APP_FUNC>* oper, bvxm_voxel_grid<vnl_vector_fixed<float,3> >* out_grid,
                            bvxm_voxel_grid<vnl_vector_fixed<int,3> >* id_grid);

  //: Non maxima suppression for an individual feature
  template<class T>
  void local_non_maxima_suppression(bvxm_voxel_grid<T>* grid_in,
                                    bvpl_kernel_sptr kernel);

  //: Keeps the n top responses after non-maxima suppression
  template<unsigned n_responses>
  void  keep_top_responses(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* top_resp_grid,
                           bvxm_voxel_grid<float>* grid_in,
                           bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
                           int this_id);

  //: Keeps the n top responses after non-maxima suppression
  template<unsigned n_responses>
  void  keep_top_responses(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* top_resp_grid,
                           bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >* grid_in,
                           bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
                           int this_id);
};


//: Applies a one kernel at a time, performs non-maxima suppression and combines the top three responses at every  voxel
template<class T, class F>
void bvpl_discriminative_non_max_suppression::suppress_and_combine(bvxm_voxel_grid<T>* grid_in,
                                                                   bvpl_kernel_vector_sptr kernel_vector,
                                                                   bvpl_neighb_operator<T,F>* oper,
                                                                   bvxm_voxel_grid<vnl_vector_fixed<float,3> >* out_grid,
                                                                   bvxm_voxel_grid<vnl_vector_fixed<int,3> >* id_grid)
{
  bvpl_local_max_functor<T> func_max;
  //create temporary grids

  if (vul_file::exists("sc_temp_response.vox"))
    vpl_unlink("sc_temp_response.vox");

  bvxm_voxel_grid<T> temp_grid("sc_temp_response.vox",grid_in->grid_size());
  temp_grid.initialize_data(func_max.min_response());
  out_grid->initialize_data(vnl_vector_fixed<float,3>(0.0f));
  id_grid->initialize_data(vnl_vector_fixed<int,3>(-1));

  for (unsigned id = 0; id < kernel_vector->kernels_.size(); ++id)
  {
    bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
    std::cout << "Processing axis: "  << kernel->axis() << std::endl
             << "Processing angle: " << kernel->angle() << std::endl;
#if 0
             << "Processing scale: " << kernel->scale() << std::endl;
#endif

    // run kernel on the grid
    oper->operate(grid_in, kernel, &temp_grid);

    //perform non-maxima suppression
    local_non_maxima_suppression(&temp_grid, kernel);
    //keep top 3 responses
    keep_top_responses(out_grid, &temp_grid, id_grid, id);
  }

  vpl_unlink("sc_temp_response.vox");
}

//: Using appearance and occupancy grids simultaneously, this function applies one kernel at a time, performs non-maxima suppression and combines the top three responses at every voxel
template<class OCP_FUNC, class APP_FUNC>
void bvpl_discriminative_non_max_suppression::suppress_and_combine(bvxm_voxel_grid<float>* ocp_grid,
                                                                   bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >* app_grid,
                                                                   bvpl_kernel_vector_sptr kernel_vector,
                                                                   bvpl_combined_neighb_operator<OCP_FUNC,APP_FUNC>* oper,
                                                                   bvxm_voxel_grid<vnl_vector_fixed<float,3> >* out_grid,
                                                                   bvxm_voxel_grid<vnl_vector_fixed<int,3> >* id_grid)
{
  bvpl_local_max_functor<float> func_max;
  //create temporary grids

  if (vul_file::exists("sc_temp_response.vox"))
    vpl_unlink("sc_temp_response.vox");

  bvxm_voxel_grid<float> temp_grid("sc_temp_response.vox",ocp_grid->grid_size());
  temp_grid.initialize_data(func_max.min_response());
  out_grid->initialize_data(vnl_vector_fixed<float,3>(0.0f));
  id_grid->initialize_data(vnl_vector_fixed<int,3>(-1));

  for (unsigned id = 0; id < kernel_vector->kernels_.size(); ++id)
  {
    bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
    std::cout << "Processing axis: "  << kernel->axis() << std::endl
             << "Processing angle: " << kernel->angle() << std::endl;
#if 0
             << "Processing scale: " << kernel->scale() << std::endl;
#endif

    // run kernel on the grid
    oper->operate(ocp_grid, app_grid, kernel, &temp_grid);

    //perform non-maxima suppression
    local_non_maxima_suppression(&temp_grid, kernel);
    //keep top 3 responses
    keep_top_responses(out_grid, &temp_grid, id_grid, id);
  }

  vpl_unlink("sc_temp_response.vox");
}


//: local non-max suppression, the result is stored in the input grid, as previous results are necessary
template<class T>
void bvpl_discriminative_non_max_suppression::local_non_maxima_suppression(bvxm_voxel_grid<T>* grid_in,
                                                                           bvpl_kernel_sptr kernel)
{
  bvpl_kernel_iterator kernel_iter = kernel->iterator();
  bvpl_subgrid_iterator<T> response_grid_iter(grid_in, kernel->min_point(), kernel->max_point());
  bvpl_local_max_functor<T> func_max;

  while (!response_grid_iter.isDone())
  {
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
  }
}


template<unsigned n_responses>
void  bvpl_discriminative_non_max_suppression::keep_top_responses(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* top_resp_grid,
                                                                  bvxm_voxel_grid<float>* grid_in,
                                                                  bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
                                                                  int this_id)
{
  bvxm_voxel_grid<float>::iterator grid_in_it = grid_in->begin();
  typename bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >::iterator top_resp_grid_it = top_resp_grid->begin();
  typename bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >::iterator id_grid_it = id_grid->begin();

  for (; grid_in_it!=grid_in->end(); ++grid_in_it, ++top_resp_grid_it, ++id_grid_it)
  {
    bvxm_voxel_slab<float>::iterator slab_in_it = grid_in_it->begin();
    typename bvxm_voxel_slab<vnl_vector_fixed<float, n_responses> >::iterator slab_resp_it = top_resp_grid_it->begin();
    typename bvxm_voxel_slab<vnl_vector_fixed<int, n_responses> >::iterator id_slab_it = id_grid_it->begin();
    for (; slab_in_it!=grid_in_it->end(); ++slab_in_it, ++ slab_resp_it, ++ id_slab_it)
    {
      float this_response = (*slab_in_it);
      vnl_vector_fixed<float,n_responses> top_responses = *slab_resp_it;
      vnl_vector_fixed<int,n_responses> top_id = *id_slab_it;
      //code to compare and keep only to three responses
      vnl_vector_fixed<float,n_responses> new_responses = top_responses;
      vnl_vector_fixed<int,n_responses> new_id = top_id;
      new_id = top_id;
      for (unsigned i=0; i<n_responses; i++)
      {
        if (this_response >= top_responses[i])
        {
          new_responses[i] = this_response;
          new_id[i] = this_id;
          for (unsigned j=i+1; j<n_responses; j++)
          {
            new_responses[j]=top_responses[j-1];
            new_id[j]= top_id[j-1];
          }
          //exit loop
          i=n_responses;
        }
      }

      //store the result
      *slab_resp_it = new_responses;
      *id_slab_it = new_id;
    }
  }
}

template<unsigned n_responses>
void  bvpl_discriminative_non_max_suppression::keep_top_responses(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* top_resp_grid,
                                                                  bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >* grid_in,
                                                                  bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
                                                                  int this_id)
{
  bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >::iterator grid_in_it = grid_in->begin();
  typename bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >::iterator top_resp_grid_it = top_resp_grid->begin();
  typename bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >::iterator id_grid_it = id_grid->begin();

  for (; grid_in_it!=grid_in->end(); ++grid_in_it, ++top_resp_grid_it, ++id_grid_it)
  {
    bvxm_voxel_slab<bsta_num_obs<bsta_gauss_sf1> >::iterator slab_in_it = grid_in_it->begin();
    typename bvxm_voxel_slab<vnl_vector_fixed<float, n_responses> >::iterator slab_resp_it = top_resp_grid_it->begin();
    typename bvxm_voxel_slab<vnl_vector_fixed<int, n_responses> >::iterator id_slab_it = id_grid_it->begin();
    for (; slab_in_it!=grid_in_it->end(); ++slab_in_it, ++ slab_resp_it, ++ id_slab_it)
    {
      float this_response = (*slab_in_it).mean();
      vnl_vector_fixed<float,n_responses> top_responses = *slab_resp_it;
      vnl_vector_fixed<int,n_responses> top_id = *id_slab_it;
      //code to compare and keep only to three responses
      vnl_vector_fixed<float,n_responses> new_responses = top_responses;
      vnl_vector_fixed<int,n_responses> new_id = top_id;
      new_id = top_id;
      for (unsigned i=0; i<n_responses; i++)
      {
        if (this_response >= top_responses[i])
        {
          new_responses[i] = this_response;
          new_id[i] = this_id;
          for (unsigned j=i+1; j<n_responses; j++)
          {
            new_responses[j]=top_responses[j-1];
            new_id[j]= top_id[j-1];
          }
          //exit loop
          i=n_responses;
        }
      }

      //store the result
      *slab_resp_it = new_responses;
      *id_slab_it = new_id;
    }
  }
}


#endif // bvpl_discriminative_non_max_suppression_h
