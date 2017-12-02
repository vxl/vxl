#ifndef boxm2_ocl_update_sky_h_
#define boxm2_ocl_update_sky_h_
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

//: boxm2_ocl_paint_batch class
class boxm2_ocl_update_sky
{
  public:
    static bool update_sky( boxm2_scene_sptr         scene,
                        bocl_device_sptr         device,
                        boxm2_opencl_cache_sptr  opencl_cache,
                        vpgl_camera_double_sptr  cam,
                        vil_image_view_base_sptr sky_img);



  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, std::string opts="", bool isRGB = false);

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;

};
class boxm2_ocl_update_sky2
{
  public:
    static bool update_sky2( boxm2_scene_sptr         scene,
                             bocl_device_sptr         device,
                             boxm2_opencl_cache_sptr  opencl_cache);

    static bool accumulate_sky_evidence( boxm2_scene_sptr         scene,
                                          bocl_device_sptr         device,
                                          boxm2_opencl_cache_sptr  opencl_cache,
                                          vpgl_camera_double_sptr  cam,
                                          vil_image_view_base_sptr sky_img);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, std::string opts="", bool isRGB = false);

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;

};
#endif // boxm2_ocl_update_sky_h_
