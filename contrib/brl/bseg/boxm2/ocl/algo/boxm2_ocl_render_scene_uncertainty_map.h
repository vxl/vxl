#ifndef boxm2_ocl_render_scene_uncertainty_map_h_
#define boxm2_ocl_render_scene_uncertainty_map_h_
//:
// \file
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//boxm2 includes
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <vnl/vnl_float_4.h>
#include <vil/vil_image_view.h>

//: boxm2_ocl_render_scene_uncertainty_map class
class boxm2_ocl_render_scene_uncertainty_map
{
  public:
    static bool render_scene_uncertainty_map( boxm2_scene_sptr & scene,
                                              bocl_device_sptr & device,
                                              boxm2_opencl_cache_sptr & opencl_cache,
                                              unsigned ni,
                                              unsigned nj,
                                              const std::string& ident,
                                              vil_image_view<float> * exp_image,
                                              vil_image_view<unsigned char> * vis_image,
                                              std::string cam_dir_1,
                                              std::string cam_dir_2);
  private:
    static   vnl_float_4 compute_cubic_trajectory(float phi_min,float phi_max,
                                                  float theta_min, float theta_max,
                                                  vil_image_view<float> * uimg);
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts="" );

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;
};

#endif // boxm2_ocl_render_scene_uncertainty_map_h_
