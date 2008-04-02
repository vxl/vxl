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
  //input[1]: The voxel world
  //input[2]: The apperance model type :this input must be either apm_mog_grey or apm_mog_rgb
  //          any other string will initialize the value for apm_mog_grey
  //input[3]: The bin index to be updatet
  input_data_.resize(4,brdb_value_sptr(0));
  input_types_.resize(4);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "bvxm_voxel_world_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "unsigned";

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

  brdb_value_t<bvxm_voxel_world_sptr>* input1 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[1].ptr());
  bvxm_voxel_world_sptr world = input1->value();

  brdb_value_t<vcl_string>* input2 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[2].ptr());
  vcl_string voxel_type = input2->value();

  brdb_value_t<unsigned>* input3 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[3].ptr());
  unsigned bin_index = input3->value();

  //create image metadata object (no image with camera, so just use dummy):
  vil_image_view_base_sptr dummy_img;
  bvxm_image_metadata camera_metadata(dummy_img,camera);
  //render image
  bool result;
  vil_image_view_base_sptr expected_img;
  vil_image_view<float> *mask;

  if (voxel_type == "apm_mog_rgb"){
    expected_img = new vil_image_view<vxl_byte>(1280,720,3);
    vil_image_view<float> mask_img(expected_img->ni(),expected_img->nj(),1);
    result = world->expected_image<APM_MOG_RGB>(camera_metadata, expected_img, mask_img, bin_index);
    mask = new vil_image_view<float>(mask_img);
  }
  else {
    expected_img = new vil_image_view<vxl_byte>(200,200,1);
    vil_image_view<float> mask_img(expected_img->ni(),expected_img->nj(),1);
    result = world->expected_image<APM_MOG_GREY>(camera_metadata, expected_img, mask_img, bin_index);
    mask = new vil_image_view<float>(mask_img);
  }

   
  vil_image_view_base_sptr mask_sptr = mask;

  //store output
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(expected_img);
  output_data_[0] = output0;

  brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(mask_sptr);
  output_data_[1] = output1;

  return result;
}

