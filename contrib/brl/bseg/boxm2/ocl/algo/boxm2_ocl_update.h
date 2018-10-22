#ifndef boxm2_ocl_update_h_
#define boxm2_ocl_update_h_
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
class boxm2_ocl_update
{
  public:
    static bool update( boxm2_scene_sptr         scene,
                        bocl_device_sptr         device,
                        boxm2_opencl_cache_sptr  opencl_cache,
                        vpgl_camera_double_sptr  cam,
                        vil_image_view_base_sptr img,
                        std::string               in_identifier="",
                        vil_image_view_base_sptr mask=nullptr,
                        bool                     update_alpha = true,
                        float                    mog_var = -1.0f,
                        bool                     update_app = true,
                        float resnearfactor = 100000.0,
                        float resfarfactor = 100000.0,
                        std::size_t               startI=0,
                        std::size_t               startJ=0);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, std::string opts="", bool isRGB = false);

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;

    //create an image buffer
    static float* prep_image_buffer(vil_image_view_base_sptr floatImg, bool isRGB, int& numFloats);

    //helper method to validate appearances
    static bool validate_appearances(boxm2_scene_sptr scene,
                                     std::string& data_type,
                                     int& appTypeSize,
                                     std::string& nobs_type,
                                     std::string& options,
                                     bool& isRGB);
};

#endif // boxm2_ocl_update_h_
