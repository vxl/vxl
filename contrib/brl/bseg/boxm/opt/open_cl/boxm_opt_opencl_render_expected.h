#ifndef boxm_opt_opencl_render_expected_h_
#define boxm_opt_opencl_render_expected_h_
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_rt_sample.h>
#include <boxm/boxm_simple_grey_processor.h>
#include <boxm/boxm_mog_grey_processor.h>
#include <vil/vil_transform.h>

#include <bcl/bcl_cl.h>
#include <boxm/opt/open_cl/boxm_ray_trace_manager.h>

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
    +"/contrib/brl/bseg/boxm/opt/open_cl/expected_functor.cl";
  vcl_vector<vcl_string> source_fnames;
  source_fnames.push_back(expected_img_functor_fname);

  const unsigned int ni = expected.ni();
  const unsigned int nj = expected.nj();
  // render the image using the opencl raytrace manager
  boxm_ray_trace_manager<boxm_sample<APM> >* ray_mgr = boxm_ray_trace_manager<boxm_sample<APM> >::instance();
  ray_mgr->init_raytrace(&scene, cam, ni, nj, source_fnames);
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
      ++results_p; // alpha integral
      ++results_p; // vis_inf
      expected(i,j) = *results_p++; // expected intensity
      mask(i,j) = *results_p++; // 1 - vis_inf
    }
  }

  ray_mgr->clean_raytrace();

  typedef typename boxm_apm_traits<APM>::obs_datatype obs_datatype;
  normalize_expected_functor<obs_datatype> norm_fn(use_black_background);
  vil_transform2<float,obs_datatype, normalize_expected_functor<obs_datatype> >(mask,expected,norm_fn);
}


#endif

