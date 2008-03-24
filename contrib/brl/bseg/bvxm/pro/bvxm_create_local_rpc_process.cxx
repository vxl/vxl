#include "bvxm_create_local_rpc_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <bil/algo/bil_cedt.h>
#include <vpgl/vpgl_local_rational_camera.h>

bvxm_create_local_rpc_process::bvxm_create_local_rpc_process()
{
  // process takes 2 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";

  // process has 1 output:
  // output[0]: The local camera
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0] = "vpgl_camera_double_sptr";
}

bool bvxm_create_local_rpc_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs
  // voxel world
  brdb_value_t<bvxm_voxel_world_sptr>* input0 = static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[0].ptr());
  // camera
  brdb_value_t<vpgl_camera_double_sptr>* input1 =  static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());

  bvxm_voxel_world_sptr vox_world = input0->value();

  vpgl_camera_double_sptr camera_inp = input1->value();
  vpgl_rational_camera<double> *cam_inp;
  if (!(cam_inp = dynamic_cast<vpgl_rational_camera<double>*>(camera_inp.ptr()))) {
    vcl_cerr << "error: process expects camera to be a vpgl_rational_camera." << vcl_endl;
    return false;
  }

  // create the local rational camera using the voxel model and the rational camera
  bgeo_lvcs_sptr lvcs = vox_world->get_params()->lvcs();
  vpgl_local_rational_camera<double> cam_out(*lvcs,*cam_inp);

  vpgl_camera_double_sptr camera_out = new vpgl_local_rational_camera<double>(cam_out);

  // update the camera and store
  brdb_value_sptr output0 = new brdb_value_t<vpgl_camera_double_sptr>(camera_out);
  output_data_[0] = output0;

  return true;
}
