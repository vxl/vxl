#ifndef boxm_ocl_render_expected_h_
#define boxm_ocl_render_expected_h_
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_rt_sample.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <vil/vil_transform.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>

#include <bocl/bocl_cl.h>
#include "boxm_ray_trace_manager.h"
#include <boxm/ocl/boxm_stack_ray_trace_manager.h>
#include <boxm/ocl/boxm_ray_bundle_trace_manager.h>
#include <boxm/ocl/boxm_render_image_manager.h>
#include <vcl_where_root_dir.h>
#include <vcl_iostream.h>

//: Functor class to normalize expected image
template<class T_obs>
class normalize_expected_functor
{
 public:
  normalize_expected_functor(bool use_black_background) : use_black_background_(use_black_background) {}

  void operator()(float mask, T_obs &pix) const
  {
    if (!use_black_background_) {
      pix += (1.0f - mask)*0.5f;
    }
  }
  bool use_black_background_;
};


template <boxm_apm_type APM>
void boxm_opencl_render_expected(boxm_scene<boct_tree<short, boxm_sample<APM> > > &scene,
                                 vpgl_camera_double_sptr cam,
                                 vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected,
                                 vil_image_view<float> & mask,
                                 bool use_black_background = false)
{
  // set up the application-specific function to be called at every cell along a ray
  vcl_string expected_img_functor_fname = vcl_string(VCL_SOURCE_ROOT_DIR)
    +"/contrib/brl/bseg/boxm/ocl/expected_functor.cl";
  vcl_vector<vcl_string> source_fnames;
  source_fnames.push_back(expected_img_functor_fname);

  const unsigned int ni = expected.ni();
  const unsigned int nj = expected.nj();
  vil_image_view<float> img0(ni,nj);
  vil_image_view<float> img1(ni,nj);
  // render the image using the opencl raytrace manager
  boxm_ray_trace_manager<boxm_sample<APM> >* ray_mgr = boxm_ray_trace_manager<boxm_sample<APM> >::instance();
  vcl_cout<<"In expected image "<<vcl_endl;

  ray_mgr->init_raytrace(&scene, cam, ni, nj, source_fnames,0,0,0);

  ray_mgr->run();

  // extract expected image and mask from OpenCL output data
  cl_float* results = ray_mgr->ray_results();
  if (!results) {
    vcl_cerr << "Error : boxm_opencl_render_expected : ray_mgr->ray_results() returned NULL\n";
    return;
  }
  cl_float *results_p = results;
  for (unsigned i = 0; i<ni; ++i) {
    for (unsigned j = 0; j<nj; ++j)  {
      img0(i,j)=*results_p++; // alpha integral
      img1(i,j)=*results_p++; // vis_inf

      expected(i,j) = *results_p++; // expected intensity
      mask(i,j) = *results_p++; // 1 - vis_inf
    }
  }
  float sum=0.0;
  vil_math_sum<float>(sum,mask,0);
  vcl_cout<<"Data transferred from global memory "<<16/* for int4*4*/*sum/1000000000.0f<<" GBytes "<<vcl_endl;
#if 1 //images for debuggin
  vil_save(img0,"f:/apl/img0.tiff");
  vil_save(img1,"f:/apl/img1.tiff");
  vil_save(expected,"f:/apl/img2.tiff");
  vil_save(mask,"f:/apl/img3.tiff");
#endif
  ray_mgr->clean_raytrace();

  typedef typename boxm_apm_traits<APM>::obs_datatype obs_datatype;
  normalize_expected_functor<obs_datatype> norm_fn(use_black_background);
  vil_transform2<float,obs_datatype, normalize_expected_functor<obs_datatype> >(mask,expected,norm_fn);
}

template <boxm_apm_type APM>
void boxm_opencl_stack_render_expected(boxm_scene<boct_tree<short, boxm_sample<APM> > > &scene,
                                       vpgl_camera_double_sptr cam,
                                       vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected,
                                       vil_image_view<float> & mask,
                                       bool use_black_background = false)
{
  // set up the application-specific function to be called at every cell along a ray
  vcl_string expected_img_functor_fname = vcl_string(VCL_SOURCE_ROOT_DIR)
    +"/contrib/brl/bseg/boxm/ocl/expected_functor.cl";
  vcl_vector<vcl_string> source_fnames;
  source_fnames.push_back(expected_img_functor_fname);

  const unsigned int ni = expected.ni();
  const unsigned int nj = expected.nj();
  // render the image using the opencl raytrace manager
  boxm_stack_ray_trace_manager<boxm_sample<APM> >* ray_mgr = boxm_stack_ray_trace_manager<boxm_sample<APM> >::instance();
  ray_mgr->init_raytrace(&scene, cam, ni, nj, source_fnames);
  ray_mgr->run();
  vil_image_view<float> img0(ni,nj);
  vil_image_view<float> img1(ni,nj);

  // extract expected image and mask from OpenCL output data
  cl_float* results = ray_mgr->ray_results();
  if (!results) {
    vcl_cerr << "Error : boxm_opencl_render_expected : ray_mgr->ray_results() returned NULL\n";
    return;
  }
  cl_float *results_p = results;
  for (unsigned i = 0; i<ni; ++i) {
    for (unsigned j = 0; j<nj; ++j)  {
      img0(i,j)=*results_p++; // alpha integral
      img1(i,j)=*results_p++; // vis_inf
      expected(i,j) = *results_p++; // expected intensity
      mask(i,j) = *results_p++; // 1 - vis_inf
    }
  }
#if 1 //images for debuggin
  vil_save(img0,"f:/apl/img0.tiff");
  vil_save(img1,"f:/apl/img1.tiff");
  vil_save(expected,"f:/apl/img2.tiff");
  vil_save(mask,"f:/apl/img3.tiff");
#endif
  float sum=0.0;
  vil_math_sum<float>(sum,mask,0);
  vcl_cout<<"Data transferred from global memory "<<16/* for int4*4*/*sum/1000000000.0f<<" GBytes "<<vcl_endl;

  ray_mgr->clean_raytrace();

  typedef typename boxm_apm_traits<APM>::obs_datatype obs_datatype;
  normalize_expected_functor<obs_datatype> norm_fn(use_black_background);
  vil_transform2<float,obs_datatype, normalize_expected_functor<obs_datatype> >(mask,expected,norm_fn);
}

template <boxm_apm_type APM>
void boxm_opencl_ray_bundle_expected(boxm_scene<boct_tree<short, boxm_sample<APM> > > &scene,
                                     vpgl_camera_double_sptr cam,
                                     vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected,
                                     vil_image_view<float> & mask,
                                     bool use_black_background = false)
{
  // set up the application-specific function to be called at every cell along a ray
  vcl_string expected_img_functor_fname = vcl_string(VCL_SOURCE_ROOT_DIR)
    +"/contrib/brl/bseg/boxm/ocl/expected_functor.cl";
  vcl_vector<vcl_string> source_fnames;
  source_fnames.push_back(expected_img_functor_fname);

  const unsigned int ni = expected.ni();
  const unsigned int nj = expected.nj();
  vil_image_view<float> img0(ni,nj);
  vil_image_view<float> img1(ni,nj);
  // render the image using the opencl raytrace manager
  boxm_ray_bundle_trace_manager<boxm_sample<APM> >* ray_mgr = boxm_ray_bundle_trace_manager<boxm_sample<APM> >::instance();
  int bundle_dim=8;
  ray_mgr->set_bundle_ni(bundle_dim);
  ray_mgr->set_bundle_nj(bundle_dim);
  ray_mgr->set_work_space_ni((int)RoundUp(ni,bundle_dim));
  ray_mgr->set_work_space_nj((int)RoundUp(nj,bundle_dim));
  ray_mgr->init_raytrace(&scene, cam, ni, nj, source_fnames,false);

  ray_mgr->run();

  // extract expected image and mask from OpenCL output data
  cl_float* results = ray_mgr->ray_results();
  if (!results) {
    vcl_cerr << "Error : boxm_opencl_render_expected : ray_mgr->ray_results() returned NULL\n";
    return;
  }
  cl_float *results_p = results;
  for (unsigned j = 0; j<nj; ++j)  {
    for (unsigned i = 0; i<ni; ++i) {
      img0(i,j)=*(results_p++); // alpha integral
      img1(i,j)=*(results_p++); // vis_inf

      expected(i,j) = *(results_p++); // expected intensity
      mask(i,j) = *(results_p++); // 1 - vis_inf
    }
  }
  float sum=0.0;
  vil_math_sum<float>(sum,mask,0);
  vcl_cout<<"Data transferred from global memory "<<16/* for int4*4*/*sum/1000000000.0f<<" GBytes "<<vcl_endl;

#if 1 //images for debuggin
  vil_save(img0,"f:/apl/img0.tiff");
  vil_save(img1,"f:/apl/img1.tiff");
  vil_save(expected,"f:/apl/img2.tiff");
  vil_save(mask,"f:/apl/img3.tiff");
#endif
  ray_mgr->clean_raytrace();

  typedef typename boxm_apm_traits<APM>::obs_datatype obs_datatype;
  normalize_expected_functor<obs_datatype> norm_fn(use_black_background);
  vil_transform2<float,obs_datatype, normalize_expected_functor<obs_datatype> >(mask,expected,norm_fn);
}


template <boxm_apm_type APM>
void boxm_opencl_all_blocks_expected(boxm_scene<boct_tree<short, boxm_sample<APM> > > &scene,
                                     vpgl_camera_double_sptr cam,
                                     vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected,
                                     vil_image_view<float> & mask,
                                     bool use_black_background = false)
{
  // set up the application-specific function to be called at every cell along a ray

  const unsigned int ni = expected.ni();
  const unsigned int nj = expected.nj();
  vil_image_view<float> img0(ni,nj);
  vil_image_view<float> img1(ni,nj);
  // render the image using the opencl raytrace manager
  boxm_render_image_manager<boxm_sample<APM> >* ray_mgr = boxm_render_image_manager<boxm_sample<APM> >::instance();
  int bundle_dim=8;
  ray_mgr->set_bundle_ni(bundle_dim);
  ray_mgr->set_bundle_nj(bundle_dim);
  //ray_mgr->set_work_space_ni((int)RoundUp(ni,bundle_dim));
  //ray_mgr->set_work_space_nj((int)RoundUp(nj,bundle_dim));
  ray_mgr->init_ray_trace(&scene, cam, expected);

  ray_mgr->run_scene();

  // extract expected image and mask from OpenCL output data
  cl_float* results = ray_mgr->output_image();
  if (!results) {
    vcl_cerr << "Error : boxm_opencl_render_expected : ray_mgr->ray_results() returned NULL\n";
    return;
  }
  cl_float *results_p = results;
  for (unsigned j = 0; j<nj; ++j)  {
    for (unsigned i = 0; i<ni; ++i) {
        
      img0(i,j)=*(results_p++); // vis_inf
      img1(i,j)=*(results_p++); // vis_inf
      expected(i,j) = *(results_p++); // expected intensity
      mask(i,j) = *(results_p++); // 1 - vis_inf

      if(i==4 && j==400)
          vcl_cout<<"["<<img0(i,j)<<","<<img1(i,j)<<","<<expected(i,j)<<","<<mask(i,j)<<"]"<<vcl_endl;
    }
  }

#if 1 //images for debuggin
  vil_save(img0,"f:/apl/img0.tiff");
  vil_save(img1,"f:/apl/img1.tiff");
  vil_save(expected,"f:/apl/img2.tiff");
  vil_save(mask,"f:/apl/img3.tiff");
#endif
}


#endif
