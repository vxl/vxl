#ifndef boxm2_ocl_update_vis_score_h_included_
#define boxm2_ocl_update_vis_score_h_included_
//:
// \file
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//boxm2 includes
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/io/boxm2_cache.h>

#include <vil/vil_image_view_base.h>

//: boxm2_ocl_paint_batch class
class boxm2_ocl_update_vis_score
{
  public:
    boxm2_ocl_update_vis_score(const boxm2_scene_sptr& scene,
                               const bocl_device_sptr& device,
                               const boxm2_opencl_cache_sptr& ocl_cache,
                               bool use_surface_normals,
                               bool optimize_transfers_ = false);

    bool run( vpgl_camera_double_sptr camera,
              unsigned ni, unsigned nj,
              const std::string& apm_id="");

  void reset(const std::string& prefix_name);
  private:
    bool compile_kernels();

    bool use_surface_normals_;
    bool optimize_transfers_;
    boxm2_scene_sptr scene_;
    bocl_device_sptr device_;
    boxm2_opencl_cache_sptr ocl_cache_;

    bocl_kernel seg_len_kernel_;
    bocl_kernel update_kernel_;
};

#endif // boxm2_ocl_update_vis_score_h_included_
