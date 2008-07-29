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
  //input[0]: The observation image
  //input[1]: The camera of the observation (dummy)
  //input[2]: The voxel world
  //input[3]: scale index  
  input_data_.resize(4,brdb_value_sptr(0));
  input_types_.resize(4);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_voxel_world_sptr";
  input_types_[3] = "unsigned";

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
  brdb_value_t<vil_image_view_base_sptr>* input0 = 
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr img = input0->value();

  brdb_value_t<vpgl_camera_double_sptr>* input1 = 
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());
  vpgl_camera_double_sptr camera = input1->value();

  brdb_value_t<bvxm_voxel_world_sptr>* input2 = 
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[2].ptr());
  bvxm_voxel_world_sptr world = input2->value();

  brdb_value_t<unsigned>* input3 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[3].ptr());
  unsigned scale_idx = input3->value();

  //create metadata:
  bvxm_image_metadata observation(img,camera);

  ////update
  //vil_image_view<float> prob_map(img->ni(),img->nj(),1);
  //vil_image_view<bool> mask(img->ni(),img->nj(),1);

  bool result = true; 

  for(unsigned curr_scale=scale_idx;curr_scale<world->get_params()->max_scale();curr_scale++)
  {
    //result = result && world->update_lidar(observation, prob_map, mask,curr_scale);
  }

  if(!result){
    vcl_cerr << "error bvxm_update_edges_lidar_process: failed to update observation" << vcl_endl;
    return false;
  }

  return true;
}
