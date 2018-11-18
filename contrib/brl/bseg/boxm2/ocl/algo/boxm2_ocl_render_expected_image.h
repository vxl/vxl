#ifndef boxm2_ocl_render_expected_image_h_
#define boxm2_ocl_render_expected_image_h_

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vil/vil_image_view.h>

namespace boxm2_ocl_render_expected_image_globals
{
  bool validate_appearances(const boxm2_scene_sptr& scene,
                            std::string& data_type,
                            int& appTypeSize,
                            std::string& options);
};

class boxm2_ocl_render_expected_image
{
public:
  static bool render( vil_image_view<float>&   exp_img_out,
                      vil_image_view<float>&   vis_img_out,
                      boxm2_scene_sptr         scene,
                      bocl_device_sptr         device,
                      boxm2_opencl_cache_sptr  opencl_cache,
                      vpgl_camera_double_sptr  cam,
                      const std::string&               ident,
                      unsigned                 ni,
                      unsigned                 nj,
                      float                    nearfactor,
                      float                    farfactor,
                      std::size_t               startI=0,
                      std::size_t               startJ=0);

private:
  static std::vector<bocl_kernel*>& get_kernel(const bocl_device_sptr& device, const std::string& opts);

  static std::map<std::string, std::vector<bocl_kernel*> > kernels_;
};

#endif // boxm2_ocl_render_expected_image_h_
