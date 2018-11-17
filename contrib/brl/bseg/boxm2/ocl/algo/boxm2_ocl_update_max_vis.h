#ifndef boxm2_ocl_update_max_vis_h_
#define boxm2_ocl_update_max_vis_h_
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
class boxm2_ocl_update_max_vis
{
  public:
    static bool update_max_vis( const boxm2_scene_sptr&         scene,
                                bocl_device_sptr         device,
                                const boxm2_opencl_cache_sptr&  opencl_cache,
                                vpgl_camera_double_sptr  cam,
                                unsigned int ni,
                                unsigned int nj,
                                const vil_image_view_base_sptr& mask_sptr,
                                float resnearfactor = 1e8,
                                float resfarfactor = 1e-8);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts="");

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;


};
//: boxm2_ocl_update_cosine_angle class
class boxm2_ocl_update_cosine_angle
{
  public:
    static bool update_cosine_angle( const boxm2_scene_sptr&         scene,
                                bocl_device_sptr         device,
                                const boxm2_opencl_cache_sptr&  opencl_cache,
                                vpgl_camera_double_sptr  cam,
                                unsigned int ni,
                                unsigned int nj,
                                const vil_image_view_base_sptr& mask_sptr,
                                float resnearfactor = 1e8,
                                float resfarfactor = 1e-8);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts="");

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;


};
//: boxm2_ocl_update_cosine_angle class
class boxm2_ocl_update_surface_density
{
  public:
    static bool update_surface_density( const boxm2_scene_sptr&         scene,
                                        bocl_device_sptr         device,
                                        const boxm2_opencl_cache_sptr&  opencl_cache,
                                        vpgl_camera_double_sptr  cam,
                                        unsigned int ni,
                                        unsigned int nj,
                                        vil_image_view<float> & exp_depth,
                                        vil_image_view<float> & std_depth,
                                        float resnearfactor = 1e8,
                                        float resfarfactor = 1e-8);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts="");

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;


};
#endif // boxm2_ocl_update_max_vis_h_
