#include "bvxm_heightmap_process.h"
#include "bvxm_voxel_world.h"

#include <vcl_cassert.h>

//#include <vil/vil_load.h>
//#include <vil/vil_image_resource.h>

#include <bprb/bprb_parameters.h>

bvxm_heightmap_process::bvxm_heightmap_process()
{
  //this process takes 3 input:
  //the filename of the image, the camera and the voxel world
  input_data_.resize(2, brdb_value_sptr(0));
  input_types_.resize(2);

  int i=0;
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world
  input_types_[i++] = "vpgl_camera_double_sptr";  // camera            
  
  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  int j=0;
  output_types_[j++]= "vil_image_view_base_sptr";  // generated image 

}

bool bvxm_heightmap_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  //voxel_world
  brdb_value_t<bvxm_voxel_world_sptr >* input0 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr >* >(input_data_[0].ptr());
  bvxm_voxel_world_sptr voxel_world = input0->value();

  // camera
  brdb_value_t<vpgl_camera_double_sptr >* input1 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr >* >(input_data_[1].ptr());
  vpgl_camera_double_sptr camera = input1->value();

  vil_image_view<unsigned> img;
  voxel_world->heightmap(camera, img);

  // store image output
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned>(img));
  output_data_[0] = output0;

}