#ifndef boxm2_ocl_update_color_h_
#define boxm2_ocl_update_color_h_
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
class boxm2_ocl_update_color
{
  public:
    static bool update_color( boxm2_scene_sptr         scene,
                              bocl_device_sptr         device,
                              boxm2_opencl_cache_sptr  opencl_cache,
                              vpgl_camera_double_sptr  cam,
                              vil_image_view_base_sptr img,
                              std::string               in_identifier="",
                              std::string               mask_filename="",
                              bool                     update_alpha=true,
                              std::size_t               startI = 0,
                              std::size_t               startJ = 0);

    static bool update_color(boxm2_scene_sptr         scene,
                             bocl_device_sptr         device,
                             boxm2_opencl_cache_sptr  opencl_cache,
                             vpgl_camera_double_sptr  cam,
                             vil_image_view_base_sptr img,
                             std::string               in_identifier="",
                             vil_image_view_base_sptr mask_img=vil_image_view_base_sptr(),
                             bool                     update_alpha=true,
                             std::size_t               startI=0,
                             std::size_t               startJ=0);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, std::string opts="" );

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;
};

#endif // boxm2_ocl_update_color_h_
