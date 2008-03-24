#include "bvxm_save_occupancy_raw_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bvxm_save_occupancy_raw_process::bvxm_save_occupancy_raw_process()
{
  // process takes 2 inputs:
  //input[0]: The voxel world
  //input[1]: The filename to write to

  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vcl_string";

  // process has 0 outputs.
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
}


bool bvxm_save_occupancy_raw_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<bvxm_voxel_world_sptr>* input0 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[0].ptr());
  bvxm_voxel_world_sptr world = input0->value();

  brdb_value_t<vcl_string>* input1 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[1].ptr());
  vcl_string filename = input1->value();

  return world->save_occupancy_raw(filename);
}


