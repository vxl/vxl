#include "bvxm_update_edges_lidar_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>


bvxm_update_edges_lidar_process::bvxm_update_edges_lidar_process()
{
  //process takes 4inputs
  //input[0]: The lidar edge height image
  //input[1]: The lidar edge prob image
  //input[2]: The camera of the observation (dummy)
  //input[3]: The voxel world
  //input[4]: scale index  
  input_data_.resize(5,brdb_value_sptr(0));
  input_types_.resize(5);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "bvxm_voxel_world_sptr";
  input_types_[4] = "unsigned";

  //output has 0 output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
}

bool bvxm_update_edges_lidar_process::execute()
{
  // Sanity check
  if(!this->verify_inputs())
    return false;

  //get the inputs
  vil_image_view_base_sptr img_height = (static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr()))->value();
  vil_image_view_base_sptr img_prob = (static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr()))->value();
  vpgl_camera_double_sptr camera = (static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[2].ptr()))->value();
  bvxm_voxel_world_sptr world = (static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[3].ptr()))->value();
  unsigned scale_idx = (static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr()))->value();

  bool result = true; 

  for(unsigned curr_scale=scale_idx;curr_scale<world->get_params()->max_scale();curr_scale++)
  {
    result = result && world->update_edges_lidar(img_height,img_prob,camera,curr_scale);
  }

  if(!result){
    vcl_cerr << "error bvxm_update_edges_lidar_process: failed to update observation" << vcl_endl;
    return false;
  }

  return true;
}
