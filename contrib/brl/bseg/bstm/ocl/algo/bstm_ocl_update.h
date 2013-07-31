#ifndef bstm_ocl_update_h_
#define bstm_ocl_update_h_
//:
// \file
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//bstm includes
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/io/bstm_cache.h>

#include <vil/vil_image_view_base.h>

//: bstm_ocl_paint_batch class
class bstm_ocl_update
{
  public:
    static bool update( bstm_scene_sptr         scene,
                          bocl_device_sptr         device,
                          bstm_opencl_cache_sptr  opencl_cache,
                          vpgl_camera_double_sptr  cam,
                          vil_image_view_base_sptr img,
                          float                   time,
                          float                    mog_var,
                          bool                     update_alpha,
                          bool                    update_changes_only,
                          vil_image_view_base_sptr mask_img);

  private:
    //compile kernels and place in static map
    static vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts="", bool isRGB = false);

    //map of paint kernel by device
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;

    //create an image buffer
    static float* prep_image_buffer(vil_image_view_base_sptr floatImg, bool isRGB, int& numFloats);

    //helper method to validate appearances
    static bool validate_appearances(bstm_scene_sptr scene,
                                         vcl_string& data_type,
                                         int& appTypeSize,
                                         vcl_string& nobs_type,
                                         vcl_string& options);
};

#endif // bstm_ocl_update_h_
