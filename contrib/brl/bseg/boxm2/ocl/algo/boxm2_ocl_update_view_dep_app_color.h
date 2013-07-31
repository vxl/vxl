#ifndef boxm2_ocl_update_view_dep_app_color_h_
#define boxm2_ocl_update_view_dep_app_color_h_
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


class boxm2_ocl_update_view_dep_app_color
{
  public:
    static bool update( boxm2_scene_sptr         scene,
                        bocl_device_sptr         device,
                        boxm2_opencl_cache_sptr  opencl_cache,
                        vpgl_camera_double_sptr  cam,
                        vil_image_view_base_sptr img,
                        vcl_string               in_identifier="",
                        vil_image_view_base_sptr mask=NULL,
                        bool                     update_alpha = true,
                        float                    mog_var = -1.0f,
                        vcl_size_t               startI=0,
                        vcl_size_t               startJ=0);

  private:
    //compile kernels and place in static map
    static vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts="");

    //map of paint kernel by device
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;

    //create an image buffer
    static float* prep_image_buffer(vil_image_view_base_sptr floatImg, int& numFloats);

    //helper method to validate appearances
    static bool validate_appearances(boxm2_scene_sptr scene,
                                     vcl_string& data_type,
                                     int& appTypeSize,
                                     vcl_string& nobs_type,
                                     vcl_string& options);
};

#endif // boxm2_ocl_update_view_dep_app_h_
