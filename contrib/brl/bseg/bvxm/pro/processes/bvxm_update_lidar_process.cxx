//This is brl/bseg/bvxm/pro/processes/bvxm_update_process.cxx
//:
// \file
// \brief A class for update process of a voxel world.
//
// \author Isabel Restrepo
// \date 04/03/2008
// \verbatim
//  Modifications
//   Brandon Mayer - 1/28/09 - converted process-class to function to conform with bvxm_process architecture.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

namespace bvxm_update_lidar_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 2;
}


bool bvxm_update_lidar_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_update_lidar_process_globals;
  
  //process takes 4inputs
  //input[0]: The observation image
  //input[1]: The camera of the observation
  //input[2]: The voxel world
  //input[3]: scale index

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_voxel_world_sptr";
  input_types_[3] = "unsigned";
  if(!pro.set_input_types(input_types_))
    return false;

  //output has 1 output
  //output[0] : The updated probability map
  //output[1] : The mask of image pixels used in update
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "vil_image_view_base_sptr";
  if(!pro.set_output_types(output_types_))
    return false;
  
  return true;
}

bool bvxm_update_lidar_process(bprb_func_process& pro)
{
  using namespace bvxm_update_lidar_process_globals;
  
  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }


  //get inputs
  unsigned i = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  unsigned scale_idx = pro.get_input<unsigned>(i++);


  if ( !img ){
    vcl_cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }
  
  if ( !camera ){
    vcl_cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if ( !world ){
    vcl_cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }
    

  //create metadata:
  bvxm_image_metadata observation(img,camera);

  //update
  vil_image_view<float> prob_map(img->ni(),img->nj(),1);
  vil_image_view<bool> mask(img->ni(),img->nj(),1);

  bool result=true;; 

  for(unsigned curr_scale=scale_idx;curr_scale<world->get_params()->max_scale();curr_scale++)
  {
    result =result && world->update_lidar(observation, prob_map, mask,curr_scale);
    if(curr_scale==scale_idx)
    {
      //Set and store outputs
      unsigned j = 0;
     pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<float>(prob_map));
     pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<bool>(mask));
    }
  }
  if(!result){
    vcl_cerr << "error bvxm_update_lidar_process: failed to update observation" << vcl_endl;
    return false;
  }

  return true;
}
