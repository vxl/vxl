// This is brl/bseg/bvxm/pro/processes/bvxm_scene_box_process.cxx
#include "bvxm_scene_box_process.h"
//:
// \file
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_util.h>
#include <vgl/vgl_point_2d.h>
#include <bprb/bprb_parameters.h>

//: set input and output types
bool bvxm_scene_box_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_scene_box_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";     // voxel world spec
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double"; // lower left lon
  output_types_[1] = "double"; // lower left lat
  output_types_[2] = "double"; // lower left elev
  output_types_[3] = "double"; // upper right lon
  output_types_[4] = "double"; // upper right lat
  output_types_[5] = "double"; // upper right elev

  return pro.set_output_types(output_types_);
}

bool bvxm_scene_box_process(bprb_func_process& pro)
{
  using namespace bvxm_scene_box_process_globals;
  //static const parameters
  static const std::string error = "error";

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs:
  unsigned i = 0;
  bvxm_voxel_world_sptr voxel_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  bvxm_world_params_sptr params = voxel_world->get_params();

  vpgl_lvcs_sptr lvcs = params->lvcs();

  double lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev;
  lvcs->local_to_global(params->corner().x(), params->corner().y(), params->corner().z(), vpgl_lvcs::wgs84,
                        lower_left_lon, lower_left_lat, lower_left_elev);
  double dimx = params->num_voxels().x() * params->voxel_length();
  double dimy = params->num_voxels().y() * params->voxel_length();
  double dimz = params->num_voxels().z() * params->voxel_length();
  lvcs->local_to_global(params->corner().x() + dimx, params->corner().y() + dimy, params->corner().z() + dimz, vpgl_lvcs::wgs84,
                        upper_right_lon, upper_right_lat, upper_right_elev);

  //Store outputs
  unsigned j = 0;
  pro.set_output_val<double>(j++, lower_left_lon);
  pro.set_output_val<double>(j++, lower_left_lat);
  pro.set_output_val<double>(j++, lower_left_elev);
  pro.set_output_val<double>(j++, upper_right_lon);
  pro.set_output_val<double>(j++, upper_right_lat);
  pro.set_output_val<double>(j++, upper_right_elev);

  return true;
}


//: set input and output types
bool bvxm_scene_origin_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types_(1);
  input_types_[0] = "bvxm_voxel_world_sptr";     // voxel world spec
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(3);
  output_types_[0] = "double"; // lower left lon
  output_types_[1] = "double"; // lower left lat
  output_types_[2] = "double"; // origin elev
  return pro.set_output_types(output_types_);
}

bool bvxm_scene_origin_process(bprb_func_process& pro)
{
  //static const parameters
  static const std::string error = "error";

  if ( pro.n_inputs() < 1 ) {
    std::cout << pro.name() << " The input number should be " << 1 << std::endl;
    return false;
  }

  // get the inputs:
  unsigned i = 0;
  bvxm_voxel_world_sptr voxel_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  bvxm_world_params_sptr params = voxel_world->get_params();

  vpgl_lvcs_sptr lvcs = params->lvcs();

  double lower_left_lon, lower_left_lat, gz, upper_right_lon, upper_right_lat;
  lvcs->local_to_global(params->corner().x(), params->corner().y(), params->corner().z(), vpgl_lvcs::wgs84, lower_left_lon, lower_left_lat, gz);

  //Store outputs
  unsigned j = 0;
  pro.set_output_val<double>(j++, lower_left_lon);
  pro.set_output_val<double>(j++, lower_left_lat);
  pro.set_output_val<double>(j++, gz);

  return true;
}
