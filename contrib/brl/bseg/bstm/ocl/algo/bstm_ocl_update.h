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
    static bool update( const bstm_scene_sptr&         scene,
                          bocl_device_sptr         device,
                          const bstm_opencl_cache_sptr&  opencl_cache,
                          vpgl_camera_double_sptr  cam,
                          const vil_image_view_base_sptr& img,
                          float                   time,
                          float                    mog_var,
                          bool                     update_alpha,
                          bool                    update_changes_only,
                          const vil_image_view_base_sptr& mask_img);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts="", bool isRGB = false);

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;

    //create an image buffer
    static float* prep_image_buffer(vil_image_view_base_sptr floatImg, bool isRGB, int& numFloats);

    //helper method to validate appearances
    static bool validate_appearances(const bstm_scene_sptr& scene,
                                         std::string& data_type,
                                         int& appTypeSize,
                                         std::string& nobs_type,
                                         std::string& options);
};

#endif // bstm_ocl_update_h_
