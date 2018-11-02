// This is brl/bseg/bvxm/pro/processes/bvxm_update_edge_tangent_process.cxx
#include <iostream>
#include <cstdio>
#include "bvxm_update_edge_tangent_process.h"
//:
// \file
// \brief A process that updates sub-voxel 3-d edge position and tangent
// \author J.L. Mundy
// \date Sept. 19, 2009

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvxm/bvxm_edge_ray_processor.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: set input and output types
bool bvxm_update_edge_tangent_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_update_edge_tangent_process_globals;

  // process takes 5 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  //input[2]: The current edge tangent image
  //input[3]: The camera of a 2nd image used only for init
  //input[4]: The a 2nd edge tangent image used only for init
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 0 outputs:
  return true;
}
//:  Initializes edge tangent world with two edge tangent images
bool bvxm_update_edge_tangent_process_init(bprb_func_process& pro)
{
  //check inputs
  if (! pro.verify_inputs()) {
    std::cerr << pro.name() << " Invalid inputs\n";
    return false;
  }
  // voxel world
  bvxm_voxel_world_sptr vox_world =
    pro.get_input<bvxm_voxel_world_sptr>(0);
  bvxm_edge_ray_processor edge_proc(vox_world);
  // camera0
  vpgl_camera_double_sptr cam0_ptr = pro.get_input<vpgl_camera_double_sptr>(1);
  // image0
  vil_image_view_base_sptr image_sptr0 =
    pro.get_input<vil_image_view_base_sptr>(2);

  bvxm_image_metadata obs0(image_sptr0, cam0_ptr);

  // camera1
  vpgl_camera_double_sptr cam1_ptr = pro.get_input<vpgl_camera_double_sptr>(3);
    // image1
  vil_image_view_base_sptr image_sptr1 =
    pro.get_input<vil_image_view_base_sptr>(4);
    bvxm_image_metadata obs1(image_sptr1, cam1_ptr);

    return edge_proc.init_von_mises_edge_tangents(obs0, obs1);
}

//:  updates 3-d edge position and tangent orientation
bool bvxm_update_edge_tangent_process(bprb_func_process& pro)
{
  using namespace bvxm_update_edge_tangent_process_globals;

  //check inputs
  if (! pro.verify_inputs()) {
    std::cerr << pro.name() << " Invalid inputs\n";
    return false;
  }
  // voxel world
  bvxm_voxel_world_sptr vox_world =
    pro.get_input<bvxm_voxel_world_sptr>(0);
  bvxm_edge_ray_processor edge_proc(vox_world);
  // camera0
  vpgl_camera_double_sptr cam0_ptr = pro.get_input<vpgl_camera_double_sptr>(1);
  // image0
  vil_image_view_base_sptr image_sptr0 =
    pro.get_input<vil_image_view_base_sptr>(2);

  bvxm_image_metadata obs0(image_sptr0, cam0_ptr);
  return edge_proc.update_von_mises_edge_tangents(obs0);
}
