// This is brl/bseg/bstm/ocl/algo/bstm_ocl_change_detection.h
#ifndef bstm_ocl_change_detection_h_
#define bstm_ocl_change_detection_h_
//:
// \file
// \brief  A process for change detection
//
// \author Ali Osman Ulusoy
// \date May 15, 2013

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <bstm/bstm_util.h>
#include <vil/vil_image_view.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace bstm_ocl_change_detection_globals
{
  const float PROB_THRESH = 0.1f;
}

//Older, single pass, multi-res change detection
class bstm_ocl_change_detection
{
  public:
    static bool change_detect(vil_image_view<float>&    change_img,
                              bocl_device_sptr          device,
                              bstm_scene_sptr          scene,
                              bstm_opencl_cache_sptr   opencl_cache,
                              vpgl_camera_double_sptr   cam,
                              vil_image_view_base_sptr  img,
                              vil_image_view_base_sptr  mask_img,
                              vcl_string                norm_type,
                              float                     time);

  private:
    static vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts);
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;
};

//Older, single pass, multi-res change detection
class bstm_ocl_update_change
{
  public:
    static bool update_change( vil_image_view<float>&    change_img,
                                  bocl_device_sptr          device,
                                  bstm_scene_sptr          scene,
                                  bstm_opencl_cache_sptr   opencl_cache,
                                  vpgl_camera_double_sptr   cam,
                                  vil_image_view_base_sptr  img,
                                  vil_image_view_base_sptr  mask_img,
                                  float                     time );

  private:
    static vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts, bool isColor);
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;
};

class bstm_ocl_aux_pass_change
{
  public:
    static bool change_detect(  vil_image_view<float>&    change_img,
                                bocl_device_sptr          device,
                                bstm_scene_sptr          scene,
                                bstm_opencl_cache_sptr   opencl_cache,
                                vpgl_camera_double_sptr   cam,
                                vil_image_view_base_sptr  img,
                                vil_image_view_base_sptr  mask_img,
                                float                     time);

  private:
    static vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts);
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;
};

#endif // bstm_ocl_change_detection_h_
