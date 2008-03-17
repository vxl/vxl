#include "bvxm_create_normalized_image_process.h"
#include "bvxm_normalize_image_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>


bvxm_create_normalized_image_process::bvxm_create_normalized_image_process()
{
  //inputs
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "float";  // input a 
  input_types_[2] = "float";  // input b

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
}


bool bvxm_create_normalized_image_process::execute()
{

  // Sanity check
  if(!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<vil_image_view_base_sptr>* input0 = 
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());

  vil_image_view_base_sptr input_img = input0->value();

  brdb_value_t<float>* input2 = 
    static_cast<brdb_value_t<float>* >(input_data_[1].ptr());
  float a = input2->value();

  brdb_value_t<float>* input3 = 
    static_cast<brdb_value_t<float>* >(input_data_[2].ptr());
  float b = input3->value();

  //: CAUTION: Assumption: Input image is of type vxl_byte
  if (input_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
    vcl_cout << "In bvxm_create_normalized_image_process::execute() -- Input image pixel format is not VIL_PIXEL_FORMAT_BYTE!\n";
    return false;
  }
  
  // return the normalized input img
  vil_image_view<vxl_byte> in_image(input_img);
  vil_image_view<vxl_byte> out_image(input_img->ni(), input_img->nj(), input_img->nplanes());
  if (!normalize_image<vxl_byte>(in_image, out_image, a, b, 255)) {
    vcl_cout << "In bvxm_create_normalized_image_process::execute() -- Problems during normalization with given inputs\n";
    return false;
  }

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out_image));
  output_data_[0] = output0;
  return true;
}


