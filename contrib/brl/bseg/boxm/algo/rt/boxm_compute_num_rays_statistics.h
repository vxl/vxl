#ifndef boxm_compute_num_rays_statistics_h
#define boxm_compute_num_rays_statistics_h

#include <iostream>
#include <boxm/boxm_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_histogram.h>
#include <boxm/basic/boxm_num_rays_functor.h>
#include <boxm/sample/boxm_scalar_sample.h>

template <class T_loc, class T_data>
bool compute_ray_statistics(boxm_scene<boct_tree<T_loc, T_data > >& scene,
                            vpgl_camera_double_sptr cam,
                            bsta_histogram<float>& num_rays_hist,
                            unsigned ni, unsigned nj)
{
  typedef boxm_aux_traits<BOXM_AUX_SCALAR_SAMPLE>::sample_datatype sample_datatype;
  boxm_aux_scene<T_loc, T_data,boxm_scalar_sample<sample_datatype> > aux_scene(&scene,boxm_aux_traits<BOXM_AUX_SCALAR_SAMPLE>::storage_subdir(), boxm_aux_scene<T_loc, T_data,boxm_scalar_sample<sample_datatype> >::CLONE);

  typedef boxm_num_rays_functor<T_data::apm_type,boxm_scalar_sample<sample_datatype> > functor_type;
  boxm_raytrace_function<functor_type,T_loc, T_data,boxm_scalar_sample<sample_datatype> > raytracer(scene,aux_scene,cam.ptr(),ni,nj);
  functor_type num_rays_functor;
  raytracer.run(num_rays_functor);

  typedef boct_tree<T_loc, T_data > tree_type;

  typedef boct_tree<T_loc, boxm_scalar_sample<sample_datatype> > aux_tree_type;
  typedef boct_tree_cell<T_loc,boxm_scalar_sample<sample_datatype> > aux_cell_type;

  num_rays_hist = bsta_histogram<float>(0.0f, 80.0f, 40);
  boxm_block_iterator<tree_type> bit = scene.iterator();
  float avg_rays=0;
  float num_cells=0;
  for (; !bit.end(); ++bit)
  {
    vgl_point_3d<int> block_index = bit.index();
    //aux_scene.load_block(block_index);
    boxm_block<aux_tree_type> * block = aux_scene.get_block(block_index);
    aux_tree_type* tree=block->get_tree();
    if (!tree) return false;
    std::vector<aux_cell_type *> leaves = tree->leaf_cells();
    for (typename std::vector<aux_cell_type *>::iterator cit = leaves.begin(); cit != leaves.end(); ++cit)
    {
      boxm_scalar_sample<sample_datatype>  data = (*cit)->data();
      num_rays_hist.upcount(static_cast<float>(data.scalar_sum_), 1.0f);
      avg_rays+=data.scalar_sum_;
      num_cells++;
    }
  }

  //aux_scene.clean_scene();
  std::cout<<"Avg num of rays per cell "<<avg_rays/num_cells<<std::endl
          << "[#rays]=Frequency" << '\n';

  for (unsigned int i=0; i<num_rays_hist.nbins(); i++)
  {
      std::cout<< '[' << num_rays_hist.avg_bin_value(i) << "]=" << num_rays_hist.counts(i) << '\n';
  }

  return true;
}

#endif
