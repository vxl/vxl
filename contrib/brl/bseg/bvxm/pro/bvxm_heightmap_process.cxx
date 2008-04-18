#include "bvxm_heightmap_process.h"
#include "bvxm_voxel_world.h"

#include <vcl_cassert.h>

#include <vil/vil_convert.h>
//#include <vil/vil_image_resource.h>

#include <bprb/bprb_parameters.h>

bvxm_heightmap_process::bvxm_heightmap_process()
{
  // This process has 4 inputs:
  //input[0]: The camera to render the heightmap from
  //input[1]: number of pixels (x)
  //input[2]: number of pixels (y)
  //input[3]: The voxel world
  input_data_.resize(4, brdb_value_sptr(0));
  input_types_.resize(4);

  int i=0;
  input_types_[i++] = "vpgl_camera_double_sptr";  // camera            
  input_types_[i++] = "unsigned";
  input_types_[i++] = "unsigned";
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world
  
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

  // camera
  brdb_value_t<vpgl_camera_double_sptr >* input0 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr >* >(input_data_[0].ptr());
  vpgl_camera_double_sptr camera = input0->value();

  // npixels x 
  brdb_value_t<unsigned>* input1 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[1].ptr());
  unsigned npixels_x = input1->value();

  // npixels y
  brdb_value_t<unsigned>* input2 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());
  unsigned npixels_y = input2->value();

  //voxel_world
  brdb_value_t<bvxm_voxel_world_sptr >* input3 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr >* >(input_data_[3].ptr());
  bvxm_voxel_world_sptr voxel_world = input3->value();

  vil_image_view<unsigned> *hmap = new vil_image_view<unsigned>(npixels_x, npixels_y, 1);
  voxel_world->heightmap(camera,*hmap);

  vil_image_view<vxl_byte> *hmap_byte = new vil_image_view<vxl_byte>(npixels_x, npixels_y, 1);
  vil_convert_stretch_range(*hmap, *hmap_byte);
 
  //store output
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(hmap_byte);
  output_data_[0] = output0;

  return true;
}