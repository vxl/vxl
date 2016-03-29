// This is brl/bseg/bvxm/pro/processes/bvxm_scene_lvcs_process.cxx
#include "bvxm_scene_lvcs_process.h"
//:
// \file
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bprb/bprb_parameters.h>
#include <vpgl/vpgl_lvcs_sptr.h>

// set input and output types
bool bvxm_scene_lvcs_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_scene_lvcs_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";  // voxel world sptr
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_lvcs_sptr";        // vpgl_lvcs sptr
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

// process
bool bvxm_scene_lvcs_process(bprb_func_process& pro)
{
  using namespace bvxm_scene_lvcs_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!\n";
    return false;
  }
  // get the inputs
  bvxm_voxel_world_sptr voxel_world = pro.get_input<bvxm_voxel_world_sptr>(0);
  bvxm_world_params_sptr params = voxel_world->get_params();

  vpgl_lvcs_sptr lvcs_sptr = params->lvcs();

  // store output
  pro.set_output_val<vpgl_lvcs_sptr>(0, lvcs_sptr);

  return true;
}
