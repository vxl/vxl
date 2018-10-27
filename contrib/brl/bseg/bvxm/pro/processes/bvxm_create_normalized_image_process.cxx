//This is brl/bseg/bvxm/pro/processes/bvxm_create_normalized_image_process.cxx
#include "bvxm_create_normalized_image_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>

#include <bvxm/pro/processes/bvxm_normalization_util.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <brip/brip_vil_float_ops.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>

//:sets input and output types for bvxm_create_normalized_image_process
bool bvxm_create_normalized_image_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_normalized_image_process_globals;
  //inputs
  //input 0: image
  //input 1: a-> scale
  //input 2: b-> offset
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "float";  // input a
  input_types_[2] = "float";  // input b
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]= "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//: create a normalize image
bool bvxm_create_normalized_image_process(bprb_func_process& pro)
{
  using namespace bvxm_create_normalized_image_process_globals;
  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name()<< "The number of inputs should be 3" << std::endl;
    return false;
  }
  //get inputs:
  unsigned i=0;
  vil_image_view_base_sptr input_img = pro.get_input<vil_image_view_base_sptr>(i++);
  auto a = pro.get_input<float>(i++);
  auto b = pro.get_input<float>(i++);

  //check input's validity
  if (!input_img) {
    std::cout << pro.name() <<" :--  Input0  is not valid!\n";
    return false;
  }

  //CAUTION: Assumption: Input image is of type vxl_byte
  if (input_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
    std::cout << "In bvxm_create_normalized_image_process::execute() -- Input image pixel format is not VIL_PIXEL_FORMAT_BYTE!\n";
    return false;
  }

  //return the normalized input img
  vil_image_view<vxl_byte> in_image(input_img);
  vil_image_view<vxl_byte> out_image(input_img->ni(), input_img->nj(), input_img->nplanes());
  if (!bvxm_normalization_util::normalize_image(in_image, out_image, a, b, (unsigned char)255)) {
    std::cout << "In bvxm_create_normalized_image_process::execute() -- Problems during normalization with given inputs\n";
    return false;
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_byte>(out_image));
  return true;
}
