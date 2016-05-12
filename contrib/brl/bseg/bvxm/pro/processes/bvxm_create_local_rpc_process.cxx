// This is brl/bseg/bvxm/pro/processes/bvxm_create_local_rpc_process.cxx
#include "bvxm_create_local_rpc_process.h"
//:
// \file

#include <bvxm/bvxm_voxel_world.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: initialize input and output types
bool bvxm_create_local_rpc_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_local_rpc_process_globals;
  // process takes 2 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  std::vector<std::string>  input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 output:
  // output[0]: The local camera
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";

  return pro.set_output_types(output_types_);
}

//: process that takes a world model and rational camera and returns a local rational camera
bool bvxm_create_local_rpc_process(bprb_func_process& pro)
{
  using namespace bvxm_create_local_rpc_process_globals;

  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() <<" : The input number should be "<< n_inputs_ << std::endl;
    return false;
  }

  //get inputs:
  //voxel world
  unsigned i = 0;
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(i++);

  //check input's validity
  i = 0;
  if (!vox_world) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (!camera_inp) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  vpgl_rational_camera<double> *cam_inp;
  if (!(cam_inp = dynamic_cast<vpgl_rational_camera<double>*>(camera_inp.ptr()))) {
    std::cerr << "error: process expects camera to be a vpgl_rational_camera.\n";
    return false;
  }

  // create the local rational camera using the voxel model and the rational camera
  vpgl_lvcs_sptr lvcs = vox_world->get_params()->lvcs();
  vpgl_local_rational_camera<double> cam_out(*lvcs,*cam_inp);

  //Set and Store outputs
  int j = 0;
  std::vector<std::string> output_types_(1);

  // updated camera
  pro.set_output_val<vpgl_camera_double_sptr>(j++, (new vpgl_local_rational_camera<double>(cam_out)));

  return true;
}
