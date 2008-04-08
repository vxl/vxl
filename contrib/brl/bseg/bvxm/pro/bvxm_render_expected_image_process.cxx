#include "bvxm_render_expected_image_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bvxm_render_expected_image_process::bvxm_render_expected_image_process()
{
  // process takes 3 inputs:
  //input[0]: The camera to render the expected image from
  //input[1]: number of pixels (x)
  //input[2]: number of pixels (y)
  //input[3]: The voxel world
  //input[4]: The apperance model type :this input must be either apm_mog_grey or apm_mog_rgb
  //          any other string will initialize the value for apm_mog_grey
  //input[5]: The bin index to be updated
  input_data_.resize(6,brdb_value_sptr(0));
  input_types_.resize(6);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "unsigned";
  input_types_[2] = "unsigned";
  input_types_[3] = "bvxm_voxel_world_sptr";
  input_types_[4] = "vcl_string";
  input_types_[5] = "unsigned";

  // process has 2 outputs:
  // output[0]: The rendered frame
  // output[1]: A confidence map, giving probability that each pixel was seen from the original view.
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
}


bool bvxm_render_expected_image_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;
  //get the inputs
  brdb_value_t<vpgl_camera_double_sptr>* input0 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[0].ptr());
  vpgl_camera_double_sptr camera = input0->value();

  brdb_value_t<unsigned>* input1 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[1].ptr());
  unsigned npixels_x = input1->value();

  brdb_value_t<unsigned>* input2 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());
  unsigned npixels_y = input2->value();

  brdb_value_t<bvxm_voxel_world_sptr>* input3 = 
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[3].ptr());
  bvxm_voxel_world_sptr world = input3->value();
  
  brdb_value_t<vcl_string>* input4 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[4].ptr());
  vcl_string voxel_type = input4->value();

  brdb_value_t<unsigned>* input5 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[5].ptr());
  unsigned bin_index = input5->value();


  //create image metadata object (no image with camera, so just use dummy):
  vil_image_view_base_sptr dummy_img = 0;
  bvxm_image_metadata camera_metadata(dummy_img,camera);
  //render image
  bool result;
  vil_image_view_base_sptr expected_img;
  vil_image_view<float> *mask_img = new vil_image_view<float>(npixels_x,npixels_y,1);

  if (voxel_type == "apm_mog_rgb"){  
    expected_img = new vil_image_view<vxl_byte>(npixels_x,npixels_y,3);
    result = world->expected_image<APM_MOG_RGB>(camera_metadata, expected_img, *mask_img, bin_index);
  }
  else {
    expected_img = new vil_image_view<vxl_byte>(npixels_x,npixels_y,1);
    result = world->expected_image<APM_MOG_GREY>(camera_metadata, expected_img, *mask_img, bin_index);
  }

   
  vil_image_view_base_sptr mask_sptr = mask;

  //store output
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(expected_img);
  output_data_[0] = output0;

  vil_image_view_base_sptr mask_sptr = mask_img;
  brdb_value_sptr output1 = 
    new brdb_value_t<vil_image_view_base_sptr>(mask_sptr);
  output_data_[1] = output1;

  return result;
}

