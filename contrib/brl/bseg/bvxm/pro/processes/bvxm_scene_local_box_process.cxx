// This is brl/bseg/bvxm/pro/processes/bvxm_scene_local_box_process.cxx
#include "bvxm_scene_local_box_process.h"
//:
// \file
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_util.h>
#include <vgl/vgl_point_2d.h>
#include <bprb/bprb_parameters.h>

//: set input and output types
bool bvxm_scene_local_box_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_scene_local_box_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";     // voxel world spec
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double"; // lower left x
  output_types_[1] = "double"; // lower left y
  output_types_[2] = "double"; // upper right x
  output_types_[3] = "double"; // upper right y
  output_types_[4] = "double"; // voxel size
  output_types_[5] = "double"; // lower left z
  output_types_[6] = "double"; // upper right z
  return pro.set_output_types(output_types_);
}

bool bvxm_scene_local_box_process(bprb_func_process& pro)
{
  using namespace bvxm_scene_local_box_process_globals;
  // sanity check input
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The input number should be " << n_inputs_ << std::endl;
    return false;
  }

  // get the input
  unsigned i = 0;
  bvxm_voxel_world_sptr voxel_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  bvxm_world_params_sptr params = voxel_world->get_params();

  double min_x, min_y, max_x, max_y, voxel_size, min_z, max_z;
  min_x = params->corner().x();
  min_y = params->corner().y();
  min_z = params->corner().z();
  max_x = params->num_voxels().x() * params->voxel_length() + params->corner().x();
  max_y = params->num_voxels().y() * params->voxel_length() + params->corner().y();
  max_z = params->num_voxels().z() * params->voxel_length() + params->corner().z();
  voxel_size = params->voxel_length();

  // output
  unsigned j = 0;
  pro.set_output_val<double>(j++, min_x);
  pro.set_output_val<double>(j++, min_y);
  pro.set_output_val<double>(j++, max_x);
  pro.set_output_val<double>(j++, max_y);
  pro.set_output_val<double>(j++, voxel_size);
  pro.set_output_val<double>(j++, min_z);
  pro.set_output_val<double>(j++, max_z);

  return true;
}
