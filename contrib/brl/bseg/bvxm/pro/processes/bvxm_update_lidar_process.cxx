//This is brl/bseg/bvxm/pro/processes/bvxm_update_lidar_process.cxx
#include "bvxm_update_lidar_process.h"
//:
// \file
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera_double_sptr.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bool bvxm_update_lidar_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_update_lidar_process_globals;

  //process takes 4inputs
  //input[0]: The observation image
  //input[1]: The camera of the observation
  //input[2]: The voxel world
  //input[3]: scale index
  //input[4]: use opinion ?
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_voxel_world_sptr";
  input_types_[3] = "unsigned";
  input_types_[4] = "bool";
  if (!pro.set_input_types(input_types_))
    return false;

  //output has 1 output
  //output[0] : The updated probability map
  //output[1] : The mask of image pixels used in update
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

bool bvxm_update_lidar_process(bprb_func_process& pro)
{
  using namespace bvxm_update_lidar_process_globals;

  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs
  unsigned i = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  auto scale_idx = pro.get_input<unsigned>(i++);
  bool use_opinion = pro.get_input<bool>(i++);

  if ( !img ) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if ( !camera ) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if ( !world ) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if ( !use_opinion ) {
    use_opinion = false;
  }

  //create metadata:
  bvxm_image_metadata observation(img,camera);

  //update
  vil_image_view<float> prob_map(img->ni(),img->nj(),1);
  vil_image_view<bool> mask(img->ni(),img->nj(),1);

  bool result=true;

  for (unsigned curr_scale=scale_idx;curr_scale<world->get_params()->max_scale();curr_scale++)
  {
    if (!use_opinion)
      result =result && world->update_lidar<OCCUPANCY>(observation, prob_map, mask,curr_scale);
    else {
      std::cout << "WOrking with opinion!" << std::endl;
      result =result && world->update_lidar<OCCUPANCY_OPINION>(observation, prob_map, mask,curr_scale);
    } if (curr_scale==scale_idx)
    {
      //Set and store outputs
      unsigned j = 0;
     pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<float>(prob_map));
     pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<bool>(mask));
    }
  }
  if (!result) {
    std::cerr << "error bvxm_update_lidar_process: failed to update observation\n";
    return false;
  }
  else
    return true;
}
