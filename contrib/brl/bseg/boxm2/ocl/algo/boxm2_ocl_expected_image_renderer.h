#ifndef boxm2_ocl_expected_image_renderer_h_included_
#define boxm2_ocl_expected_image_renderer_h_included_

#include <boxm2/boxm2_scene.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vil/vil_image_view.h>

class boxm2_ocl_expected_image_renderer
{
  public:
    boxm2_ocl_expected_image_renderer(boxm2_scene_sptr scene, bocl_device_sptr device, vcl_string ident="");

    bool render(vpgl_camera_double_sptr camera, unsigned ni, unsigned nj, boxm2_opencl_cache_sptr cache, float nearfactor=0.0f, float farfactor=1000000.0f);

    bool get_last_rendered(vil_image_view<float> &img);
    bool get_last_vis(vil_image_view<float> &vis_img);

  private:
    boxm2_scene_sptr scene_;
    bocl_device_sptr device_;
    vcl_string data_type_;
    int apptypesize_;
    vcl_vector<bocl_kernel*> kernels_;
    bool compile_kernels(bocl_device_sptr device, vcl_vector<bocl_kernel*> & vec_kernels, boxm2_data_type data_type);

    bool render_success_;
    vil_image_view<float> expected_img_;
    vil_image_view<float> vis_img_;
};

#endif
