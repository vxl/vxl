#ifndef boxm2_ocl_paint_online_h_
#define boxm2_ocl_paint_online_h_
//:
// \file
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//boxm2 includes
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/io/boxm2_cache.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

//: boxm2_ocl_paint_batch class
class boxm2_ocl_paint_online
{
  public:
    static bool paint_scene( const boxm2_scene_sptr&         scene,
                             const bocl_device_sptr&         device,
                             const boxm2_opencl_cache_sptr&  opencl_cache,
                             const vil_image_view_base_sptr& img,
                             const vpgl_camera_double_sptr&  cam,
                             std::string const& apm_id="");

    static bool paint_scene_with_weights( const boxm2_scene_sptr&         scene,
                                          bocl_device_sptr         device,
                                          const boxm2_opencl_cache_sptr&  opencl_cache,
                                          const vil_image_view_base_sptr& img,
                                          vil_image_view<float> const& weights,
                                          vpgl_camera_double_sptr  cam,
                                          std::string const& apm_id="");

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*> compile_kernels( const bocl_device_sptr& device, const std::string& opts="" );

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;
};

#endif // boxm2_ocl_paint_online_h_
