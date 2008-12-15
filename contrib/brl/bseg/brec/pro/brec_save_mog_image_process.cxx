#include "brec_save_mog_image_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>


brec_save_mog_image_process::brec_save_mog_image_process()
{
  //process takes 0 inputs
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]= "brec_voxel_slab_base_sptr";

  //process has 1 output
  //output[0] : The voxel_world

  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
  //output_types_[0]= "brec_voxel_world_sptr";

}

bool brec_save_mog_image_process::execute()
{
  

  return true;
}

