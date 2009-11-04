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

#include "bvpl_neighb_operator.h"
#include "bvpl_combined_neighb_operator.h"
#include "bvpl_kernel_factory.h"
#include <bvpl/bvpl_local_max_functor.h>
#include <vcl_iostream.h>
#include <vcl_limits.h>
#include <vpl/vpl.h>
#include <vul/vul_file.h>

class bvpl_vector_operator
{
 public:
  //: Applies a vector of kernel and functor to a grid.
  //  The result is a grid of maxima response and a grid and id to the winning kernel
  template<class T, class F>
  void apply_and_suppress(bvxm_voxel_grid<T>* grid, bvpl_kernel_vector_sptr kernel_vector,
                          bvpl_neighb_operator<T,F>* oper, bvxm_voxel_grid<T>* out_grid,
                          bvxm_voxel_grid<unsigned >* id_grid);

  //: Non-max suppression among all features, the result is stored in the input grid, as previous results are necessary
  template<class T>
  void non_maxima_suppression(bvxm_voxel_grid<T>* grid_in, bvxm_voxel_grid<unsigned >* id_grid,
                              bvpl_kernel_vector_sptr kernel_vectors);

  //: Non-max suppression among features of its own kind
  template<class T>
  void local_non_maxima_suppression(bvxm_voxel_grid<T>* grid_in, bvxm_voxel_grid<unsigned >* id_grid,
                                    bvpl_kernel_vector_sptr kernel_vector);

  //: Returns the response corresponding to the target id, all others are set to zero.
  template<class T>
  void filter_response(bvxm_voxel_grid<T>* grid_in, bvxm_voxel_grid<unsigned >* id_grid,
                       unsigned targed_id, bvxm_voxel_grid<float>* out_grid);

  //: Returns a a grid containing single responses from a grid containing vectors of responses
  template<unsigned n_responses>
  void get_response(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* resp_grid,
                    bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
                    unsigned idx,
                    bvxm_voxel_grid<float>* out_grid, bvxm_voxel_grid<int>* out_id_grid);

//  //: Applies a one kernel at a time, performs non-maxima suppression and combines the top three responses at every  voxel
//  template<class T, class F>
//  void suppress_and_combine(bvxm_voxel_grid<T>* grid_in,  bvpl_kernel_vector_sptr kernel_vector,
//                            bvpl_neighb_operator<T,F>* oper, bvxm_voxel_grid<vnl_vector_fixed<float,3> >* out_grid,
//                            bvxm_voxel_grid<vnl_vector_fixed<int,3> >* id_grid);
//
//  //: Using appearance and occupancy grids simultaneously, this function applies a one kernel at a time, performs non-maxima suppression and combines the top three responses at every voxel
//  template<class OCP_FUNC, class APP_FUNC>
//  void suppress_and_combine(bvxm_voxel_grid<float>* ocp_grid, bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> >* app_grid, bvpl_kernel_vector_sptr kernel_vector,
//                            bvpl_combined_neighb_operator<OCP_FUNC,APP_FUNC>* oper, bvxm_voxel_grid<vnl_vector_fixed<float,3> >* out_grid,
//                            bvxm_voxel_grid<vnl_vector_fixed<int,3> >* id_grid);

  template<class T>
  void get_max_orientation_grid(bvxm_voxel_grid<T>* out_grid, bvxm_voxel_grid<T>* temp_grid,
                                bvxm_voxel_grid<unsigned >* id_grid,
                                unsigned temp_orientation);

  //: Non maxima suppression for an individual feature, this private function is used by suppress and combine
//  template<class T>
//  void local_non_maxima_suppression(bvxm_voxel_grid<T>* grid_in,
//                                    bvpl_kernel_sptr kernel);
//
//  //: Keeps the n top responses after non-maxima suppression
//  template<unsigned n_responses>
//  void  keep_top_responses(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* top_resp_grid,
//                           bvxm_voxel_grid<float>* grid_in,
//                           bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
//                           int this_id);
//
//  //: Keeps the n top responses after non-maxima suppression
//  template<unsigned n_responses>
//  void  keep_top_responses(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* top_resp_grid,
//                           bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> >* grid_in,
//                           bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
//                           int this_id);
};


//: local non-max suppression, the result is stored in the input grid, as previous results are necessary
template<class T>
void bvpl_vector_operator::non_maxima_suppression(bvxm_voxel_grid<T>* grid_in,
                                                  bvxm_voxel_grid<unsigned >* id_grid,
                                                  bvpl_kernel_vector_sptr kernel_vector)
{
  bvpl_subgrid_iterator<T> response_grid_iter(grid_in, kernel_vector->min(), kernel_vector->max());
   bvpl_subgrid_iterator<unsigned int> kernel_id_iter(id_grid, kernel_vector->min(), kernel_vector->max());
  bvpl_local_max_functor<T> func_max;
  //kernel->print();
  while (!response_grid_iter.isDone()) {
    unsigned index=(*kernel_id_iter).get_voxel();
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
          vcl_cout << "val" << val << "at " << idx <<vcl_endl;
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
                                                        bvxm_voxel_grid<unsigned >* id_grid,
                                                        bvpl_kernel_vector_sptr kernel_vector)
{
  bvpl_subgrid_iterator<T> response_grid_iter(grid_in, kernel_vector->max_dim());
  //bvpl_subgrid_iterator<T> out_grid_iter(grid_out, kernel_vector->max_dim());
  bvpl_subgrid_iterator<unsigned int> kernel_id_iter(id_grid, kernel_vector->max_dim());

  bvpl_local_max_functor<T> func_max;
  //kernel->print();
  while (!response_grid_iter.isDone()) {
    bvpl_voxel_subgrid<T> subgrid = *response_grid_iter;
    bvpl_voxel_subgrid<unsigned> id_subgrid = *kernel_id_iter;
    unsigned index=id_subgrid.get_voxel();
    bvpl_kernel_iterator kernel_iter = kernel_vector->kernels_[index]->iterator();
    //reset the iterator
    kernel_iter.begin();
    while (!kernel_iter.isDone()) {
      vgl_point_3d<int> idx = kernel_iter.index();
      T val;
      unsigned id_val;
      //we don't want to apply to the current center voxels
      if (!((idx.x() == 0) && (idx.y() == 0) && (idx.z() == 0)))
        if (subgrid.voxel(idx, val) && id_subgrid.voxel(idx, id_val)) {
          if (id_val == index){
#if 0
            vcl_cout << "val" << val << "at " << idx <<vcl_endl;
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


//: Applies a one kernel at a time, performs non-maxima suppression and combines the top three responses at every  voxel
//template<class T, class F>
//void bvpl_vector_operator::suppress_and_combine(bvxm_voxel_grid<T>* grid_in,
//                                                bvpl_kernel_vector_sptr kernel_vector,
//                                                bvpl_neighb_operator<T,F>* oper,
//                                                bvxm_voxel_grid<vnl_vector_fixed<float,3> >* out_grid,
//                                                bvxm_voxel_grid<vnl_vector_fixed<int,3> >* id_grid)
//{
//  bvpl_local_max_functor<T> func_max;
//  //create temporary grids
//
//  if (vul_file::exists("sc_temp_response.vox"))
//    vpl_unlink("sc_temp_response.vox");
//
//
//  bvxm_voxel_grid<T> temp_grid("sc_temp_response.vox",grid_in->grid_size());
//  temp_grid.initialize_data(func_max.min_response());
//  out_grid->initialize_data(vnl_vector_fixed<float,3>(0.0f));
//  id_grid->initialize_data(vnl_vector_fixed<int,3>(-1));
//
//  for (unsigned id = 0; id < kernel_vector->kernels_.size(); ++id)
//  {
//    bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
//    vcl_cout << "Processing axis: "  << kernel->axis() << vcl_endl
//             << "Processing angle: " << kernel->angle() << vcl_endl;
//#if 0
//             << "Processing scale: " << kernel->scale() << vcl_endl;
//#endif
//
//    // run kernel on the grid
//    oper->operate(grid_in, kernel, &temp_grid);
//
//    //perform non-maxima suppression
//    local_non_maxima_suppression(&temp_grid, kernel);
//    //keep top 3 responses
//    keep_top_responses(out_grid, &temp_grid, id_grid, id);
//  }
//
//  vpl_unlink("sc_temp_response.vox");
//}
//
//: Using appearance and occupancy grids simultaneously, this function applies one kernel at a time, performs non-maxima suppression and combines the top three responses at every voxel
//template<class OCP_FUNC, class APP_FUNC>
//void bvpl_vector_operator::suppress_and_combine(bvxm_voxel_grid<float>* ocp_grid,
//                                                bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> >* app_grid,
//                                                bvpl_kernel_vector_sptr kernel_vector,
//                                                bvpl_combined_neighb_operator<OCP_FUNC,APP_FUNC>* oper,
//                                                bvxm_voxel_grid<vnl_vector_fixed<float,3> >* out_grid,
//                                                bvxm_voxel_grid<vnl_vector_fixed<int,3> >* id_grid)
//{
//  bvpl_local_max_functor<float> func_max;
//  //create temporary grids
//
//  if (vul_file::exists("sc_temp_response.vox"))
//    vpl_unlink("sc_temp_response.vox");
//
//  bvxm_voxel_grid<float> temp_grid("sc_temp_response.vox",ocp_grid->grid_size());
//  temp_grid.initialize_data(func_max.min_response());
//  out_grid->initialize_data(vnl_vector_fixed<float,3>(0.0f));
//  id_grid->initialize_data(vnl_vector_fixed<int,3>(-1));
//
//  for (unsigned id = 0; id < kernel_vector->kernels_.size(); ++id)
//  {
//    bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
//    vcl_cout << "Processing axis: "  << kernel->axis() << vcl_endl
//             << "Processing angle: " << kernel->angle() << vcl_endl;
//#if 0
//             << "Processing scale: " << kernel->scale() << vcl_endl;
//#endif
//
//    // run kernel on the grid
//    oper->operate(ocp_grid, app_grid, kernel, &temp_grid);
//
//    //perform non-maxima suppression
//    local_non_maxima_suppression(&temp_grid, kernel);
//    //keep top 3 responses
//    keep_top_responses(out_grid, &temp_grid, id_grid, id);
//  }
//
//  vpl_unlink("sc_temp_response.vox");
//}

template<class T, class F>
void bvpl_vector_operator::apply_and_suppress(bvxm_voxel_grid<T>* grid,
                                              bvpl_kernel_vector_sptr kernel_vector,
                                              bvpl_neighb_operator<T,F>* oper,
                                              bvxm_voxel_grid<T>* out_grid,
                                              bvxm_voxel_grid<unsigned>* id_grid)
{
  bvpl_local_max_functor<T> func_max;
  bvxm_voxel_grid<T> temp_grid("temp_grid.vox",grid->grid_size());
  temp_grid.initialize_data(func_max.min_response());
  out_grid->initialize_data(func_max.min_response());
  id_grid->initialize_data(0);

  for (unsigned id = 0; id < kernel_vector->kernels_.size(); ++id)
  {
    bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
    vcl_cout << "Processing axis: "  << kernel->axis() << vcl_endl
             << "Processing angle: " << kernel->angle() << vcl_endl;
#if 0
             << "Processing scale: " << kernel->scale() << vcl_endl;
#endif
    oper->operate(grid, kernel, &temp_grid);
    get_max_orientation_grid(out_grid, &temp_grid, id_grid, id);
  }

  vpl_unlink("temp_grid.vox");
}

template<class T>
void  bvpl_vector_operator::filter_response(bvxm_voxel_grid<T>* grid_in, bvxm_voxel_grid<unsigned >* id_grid,
                                            unsigned targed_id, bvxm_voxel_grid<float>* out_grid)
{
  bvpl_local_max_functor<T> func_max;
  out_grid->initialize_data(0.0f);
  bvxm_voxel_grid<float>::iterator out_grid_it = out_grid->begin();
  typename bvxm_voxel_grid<T>::iterator in_grid_it = grid_in->begin();
  bvxm_voxel_grid<unsigned >::iterator id_grid_it = id_grid->begin();

  for (; out_grid_it!=out_grid->end(); ++out_grid_it, ++in_grid_it, ++id_grid_it)
  {
    bvxm_voxel_slab<float>::iterator out_slab_it = (*out_grid_it).begin();
    typename bvxm_voxel_slab<T>::iterator in_slab_it= (*in_grid_it).begin();
    bvxm_voxel_slab<unsigned>::iterator id_slab_it = id_grid_it->begin();

    for (; out_slab_it!=(*out_grid_it).end(); ++out_slab_it, ++in_slab_it, ++id_slab_it)
    {
      *out_slab_it = func_max.filter_response(*id_slab_it, targed_id, *in_slab_it);
    }
  }
}


//: Returns a a grid containing single responses from a grid containing vectors of responses
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


//: local non-max suppression, the result is stored in the input grid, as previous results are necessary
//template<class T>
//void bvpl_vector_operator::local_non_maxima_suppression(bvxm_voxel_grid<T>* grid_in,
//                                                        bvpl_kernel_sptr kernel)
//{
//  bvpl_kernel_iterator kernel_iter = kernel->iterator();
//  bvpl_subgrid_iterator<T> response_grid_iter(grid_in, kernel->min_point(), kernel->max_point());
//  bvpl_local_max_functor<T> func_max;
//
//  while (!response_grid_iter.isDone())
//  {
//    bvpl_voxel_subgrid<T> subgrid = *response_grid_iter;
//    //reset the iterator
//    kernel_iter.begin();
//    while (!kernel_iter.isDone()) {
//      vgl_point_3d<int> idx = kernel_iter.index();
//      T val;
//      //we don't want to apply to the current center voxels
//      if (!((idx.x() == 0) && (idx.y() == 0) && (idx.z() == 0)))
//        if (subgrid.voxel(idx, val)) {
//#if 0
//          vcl_cout << "val" << val << "at " << idx <<vcl_endl;
//#endif
//          bvpl_kernel_dispatch d = *kernel_iter;
//          func_max.apply(val, d);
//        }
//      ++kernel_iter;
//    }
//
//    // set the result at the input grid
//    subgrid.set_voxel(func_max.result(subgrid.get_voxel()));
//    ++response_grid_iter;
//  }
//}


template<class T>
void bvpl_vector_operator::get_max_orientation_grid(bvxm_voxel_grid<T>* out_grid, bvxm_voxel_grid<T>* temp_grid,
                                                    bvxm_voxel_grid<unsigned>* id_grid,
                                                    unsigned id)
{
  bvpl_local_max_functor<T> func_max;
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
      if (func_max.greater_than((* temp_slab_it), (*out_slab_it)))
      {
        (*out_slab_it) =  (* temp_slab_it);
        (*id_slab_it) = id;
      }
    }
  }
}


//template<unsigned n_responses>
//void  bvpl_vector_operator::keep_top_responses(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* top_resp_grid,
//                                               bvxm_voxel_grid<float>* grid_in,
//                                               bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
//                                               int this_id)
//{
//  bvxm_voxel_grid<float>::iterator grid_in_it = grid_in->begin();
//  typename bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >::iterator top_resp_grid_it = top_resp_grid->begin();
//  typename bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >::iterator id_grid_it = id_grid->begin();
//
//  for (; grid_in_it!=grid_in->end(); ++grid_in_it, ++top_resp_grid_it, ++id_grid_it)
//  {
//    bvxm_voxel_slab<float>::iterator slab_in_it = grid_in_it->begin();
//    typename bvxm_voxel_slab<vnl_vector_fixed<float, n_responses> >::iterator slab_resp_it = top_resp_grid_it->begin();
//    typename bvxm_voxel_slab<vnl_vector_fixed<int, n_responses> >::iterator id_slab_it = id_grid_it->begin();
//    for (; slab_in_it!=grid_in_it->end(); ++slab_in_it, ++ slab_resp_it, ++ id_slab_it)
//    {
//      float this_response = (*slab_in_it);
//      vnl_vector_fixed<float,n_responses> top_responses = *slab_resp_it;
//      vnl_vector_fixed<int,n_responses> top_id = *id_slab_it;
//      //code to compare and keep only to three responses
//      vnl_vector_fixed<float,n_responses> new_responses = top_responses;
//      vnl_vector_fixed<int,n_responses> new_id = top_id;
//      new_id = top_id;
//      for (unsigned i=0; i<n_responses; i++)
//      {
//        if (this_response >= top_responses[i])
//        {
//          new_responses[i] = this_response;
//          new_id[i] = this_id;
//          for (unsigned j=i+1; j<n_responses; j++)
//          {
//            new_responses[j]=top_responses[j-1];
//            new_id[j]= top_id[j-1];
//          }
//          //exit loop
//          i=n_responses;
//        }
//      }
//
//      //store the result
//      *slab_resp_it = new_responses;
//      *id_slab_it = new_id;
//    }
//  }
//}

//template<unsigned n_responses>
//void  bvpl_vector_operator::keep_top_responses(bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >* top_resp_grid,
//                                               bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> >* grid_in,
//                                               bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >* id_grid,
//                                               int this_id)
//{
//  bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> >::iterator grid_in_it = grid_in->begin();
//  typename bvxm_voxel_grid<vnl_vector_fixed<float, n_responses> >::iterator top_resp_grid_it = top_resp_grid->begin();
//  typename bvxm_voxel_grid<vnl_vector_fixed<int, n_responses> >::iterator id_grid_it = id_grid->begin();
//
//  for (; grid_in_it!=grid_in->end(); ++grid_in_it, ++top_resp_grid_it, ++id_grid_it)
//  {
//    bvxm_voxel_slab<bsta_num_obs<bsta_gauss_f1> >::iterator slab_in_it = grid_in_it->begin();
//    typename bvxm_voxel_slab<vnl_vector_fixed<float, n_responses> >::iterator slab_resp_it = top_resp_grid_it->begin();
//    typename bvxm_voxel_slab<vnl_vector_fixed<int, n_responses> >::iterator id_slab_it = id_grid_it->begin();
//    for (; slab_in_it!=grid_in_it->end(); ++slab_in_it, ++ slab_resp_it, ++ id_slab_it)
//    {
//      float this_response = vcl_abs((*slab_in_it).mean());
//      vnl_vector_fixed<float,n_responses> top_responses = *slab_resp_it;
//      vnl_vector_fixed<int,n_responses> top_id = *id_slab_it;
//      //code to compare and keep only to three responses
//      vnl_vector_fixed<float,n_responses> new_responses = top_responses;
//      vnl_vector_fixed<int,n_responses> new_id = top_id;
//      new_id = top_id;
//      for (unsigned i=0; i<n_responses; i++)
//      {
//        if (this_response >= top_responses[i])
//        {
//          new_responses[i] = this_response;
//          new_id[i] = this_id;
//          for (unsigned j=i+1; j<n_responses; j++)
//          {
//            new_responses[j]=top_responses[j-1];
//            new_id[j]= top_id[j-1];
//          }
//          //exit loop
//          i=n_responses;
//        }
//      }
//
//      //store the result
//      *slab_resp_it = new_responses;
//      *id_slab_it = new_id;
//    }
//  }
//}


#endif // bvpl_vector_operator_h
