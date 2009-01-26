//This is brl/bseg/bvxm/pro/processes/bvxm_create_local_rpc_process.cxx

//:
// \file
#include <bprb/bprb_func_process.h>

#include "bvxm_normalize_image_process.h"
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>


bool bvxm_create_normalized_image_process(bprb_func_process& pro)
{
  //inputs
  //:input 0: image
  //:input 1: a-> scale
  //:input 2: b-> offset
  if(pro.n_inputs()<3)
  {
    vcl_cout << pro.name()<< "The number of inputs should be 3" << vcl_endl;
    return false; 
  }
  
  //get inputs:
  unsigned i=0;
  vil_image_view_base_sptr input_img = bpro.get_input<vil_image_view_base_sptr>(i++);
  float a = get_input<float>(i++);
  float b = get_input<float>(i++);
  
    //check imput's validity
  i = 0;
  if (!input_img) {
    vcl_cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }
  
  
  //: CAUTION: Assumption: Input image is of type vxl_byte
  if (input_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
    vcl_cout << "In bvxm_create_normalized_image_process::execute() -- Input image pixel format is not VIL_PIXEL_FORMAT_BYTE!\n";
    return false;
  }

  // return the normalized input img
  vil_image_view<vxl_byte> in_image(input_img);
  vil_image_view<vxl_byte> out_image(input_img->ni(), input_img->nj(), input_img->nplanes());
  if (!normalize_image(in_image, out_image, a, b, (unsigned char)255)) {
    vcl_cout << "In bvxm_create_normalized_image_process::execute() -- Problems during normalization with given inputs\n";
    return false;
  }

  //Set and store outputs
  int j = 0;
  vcl_vector<vcl_string> output_types_(1);
  //normalized image
  output_types_[j++] = "vil_image_view_base_sptr";
  pro.set_output_types(output_types_);
  
  j = 0;
  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out_image));
  pro.set_output(j++, output0);
  return true;
}


