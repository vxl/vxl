#ifndef boxm_ocl_render_expected_h_
#define boxm_ocl_render_expected_h_
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_rt_sample.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <vil/vil_image_view.h>

#include <bocl/bocl_cl.h>
#include "boxm_ray_trace_manager.h"

#include <boxm/ocl/boxm_render_ocl_scene_manager.h>
#include <boxm/ocl/boxm_ocl_bit_scene.h>
#include <boxm/ocl/boxm_render_bit_scene_manager.h>
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


void boxm_opencl_ocl_scene_expected(boxm_ocl_scene &scene,
                                    vpgl_camera_double_sptr cam,
                                    vil_image_view<float> &expected,
                                    vil_image_view<float> & mask,
                                    bool /*use_black_background*/ = false);

//: function to rerender views on adjacent frames
void boxm_ocl_scene_rerender(boxm_ocl_scene &scene,
                             vcl_vector<vcl_string> camfiles,
                             vcl_vector<vcl_string> imgfiles,
                             vcl_string out_dir);

//: function to render bit scene
void boxm_opencl_bit_scene_expected(boxm_ocl_bit_scene &scene,
                                    vpgl_camera_double_sptr cam,
                                    vil_image_view<float> &expected,
                                    vil_image_view<float> & mask,
                                    bool /*use_black_background*/ = false);
#endif
