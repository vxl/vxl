// This is brl/bseg/bvxm/pro/processes/bvxm_display_edge_tangent_world_vrml_process.cxx
#include "bvxm_display_edge_tangent_world_vrml_process.h"
//:
// \file
#include <vul/vul_file.h>
#include <bprb/bprb_func_process.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_edge_ray_processor.h>
//: set input and output types
bool bvxm_display_edge_tangent_world_vrml_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_display_edge_tangent_world_vrml_process_globals;

  // process takes 2 inputs:
  //input[0]: The edge tangent voxel world
  //input[1]: The path for the output VRML file
  //input[1]: The path for the ground truth file (if available)
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;
  // process has 0 outputs:
  return true;
}

//: renders the expected image
bool bvxm_display_edge_tangent_world_vrml_process(bprb_func_process& pro)
{
  using namespace bvxm_display_edge_tangent_world_vrml_process_globals;

  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cerr << pro.name() << " invalid inputs\n";
    return false;
  }

  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(0);
  bvxm_edge_ray_processor edge_proc(vox_world);
  std::string vrml_path = pro.get_input<std::string>(1);
  std::string gnd_truth_path = pro.get_input<std::string>(2);
  edge_proc.display_edge_tangent_world_vrml(vrml_path);
  if (gnd_truth_path != "") {
    std::string gnd_truth_path_vrml = vul_file::strip_extension(vrml_path);
    gnd_truth_path_vrml += "_gnd_truth.wrl";
    edge_proc.display_ground_truth(gnd_truth_path, gnd_truth_path_vrml);
  }
  return true;
}
