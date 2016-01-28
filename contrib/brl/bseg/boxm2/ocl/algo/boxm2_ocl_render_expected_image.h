#ifndef boxm2_ocl_render_expected_image_h_
#define boxm2_ocl_render_expected_image_h_

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vil/vil_image_view.h>

namespace boxm2_ocl_render_expected_image_globals
{
bool validate_appearances(boxm2_scene_sptr scene, vcl_string& data_type, int& appTypeSize, vcl_string& options);

};

class boxm2_ocl_render_expected_image
{
public:
  static bool render( vil_image_view<float>&   exp_img_out, vil_image_view<float>&   vis_img_out,
                      boxm2_scene_sptr         scene, bocl_device_sptr         device,
                      boxm2_opencl_cache_sptr  opencl_cache, vpgl_camera_double_sptr  cam,
                      vcl_string               ident, unsigned                 ni, unsigned                 nj,
                      float                    nearfactor, float                    farfactor,
                      vcl_size_t               startI = 0, vcl_size_t               startJ = 0);

private:
  static vcl_vector<bocl_kernel *> & get_kernel(bocl_device_sptr device, vcl_string opts);

  static vcl_map<vcl_string, vcl_vector<bocl_kernel *> > kernels_;
};

#endif // boxm2_ocl_render_expected_image_h_
