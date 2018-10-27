// This is brl/bseg/bvxm/pro/processes/bvxm_render_expected_image_process.cxx
#include "bvxm_render_expected_image_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera_double_sptr.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_edge_ray_processor.h>
//: set input and output types
bool bvxm_render_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_render_expected_image_process_globals;

  // process takes 7 inputs:
  //input[0]: The camera to render the expected image from
  //input[1]: number of pixels (x)
  //input[2]: number of pixels (y)
  //input[3]: The voxel world
  //input[4]: The appearance model type :this input must be either apm_mog_grey, apm_mog_rgb, edges or edges_prob
  //          any other string will initialize the value for apm_mog_grey
  //input[5]: The bin index to be updated
      //input[6]: The scale index to be updated

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "unsigned";
  input_types_[i++] = "unsigned";
  input_types_[i++] = "bvxm_voxel_world_sptr";
  input_types_[i++] = "vcl_string";
  input_types_[i++] = "unsigned";
  input_types_[i++] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 2 outputs:
  // output[0]: The rendered frame
  // output[1]: A confidence map, giving probability that each pixel was seen from the original view.
  std::vector<std::string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++]= "vil_image_view_base_sptr";
  output_types_[j++] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//: renders the expected image
bool bvxm_render_expected_image_process(bprb_func_process& pro)
{
  using namespace bvxm_render_expected_image_process_globals;

  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto npixels_x = pro.get_input<unsigned>(i++);
  auto npixels_y = pro.get_input<unsigned>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  std::string voxel_type = pro.get_input<std::string>(i++);
  auto bin_index = pro.get_input<unsigned>(i++);
  auto scale_index = pro.get_input<unsigned>(i++);

  //check input validity
  if (!camera) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
  if (!world) {
    std::cout << pro.name() <<" :--  Input 3  is not valid!\n";
    return false;
  }

  //create image metadata object (no image with camera, so just use dummy):
  vil_image_view_base_sptr dummy_img = nullptr;
  bvxm_image_metadata camera_metadata(dummy_img,camera);
  //render image
  bool result=true;
  vil_image_view_base_sptr expected_img;
  auto *mask_img = new vil_image_view<float>(npixels_x,npixels_y,1);

  if (voxel_type == "apm_mog_grey")
  {
    expected_img = new vil_image_view<vxl_byte>(npixels_x,npixels_y,1);
    result = world->expected_image<APM_MOG_GREY>(camera_metadata, expected_img, *mask_img, bin_index,scale_index);
  }
  else if (voxel_type == "apm_mog_rgb") {
    expected_img = new vil_image_view<vxl_byte>(npixels_x,npixels_y,3);
    result = world->expected_image<APM_MOG_RGB>(camera_metadata, expected_img, *mask_img, bin_index,scale_index);
  }
  else if (voxel_type == "apm_mog_mc_2_3") {
    expected_img = new vil_image_view<vxl_byte>(npixels_x,npixels_y,2);
    result = world->expected_image<APM_MOG_MC_2_3>(camera_metadata, expected_img, *mask_img, bin_index,scale_index);
  }
  else if (voxel_type == "apm_mog_mc_3_3") {
    expected_img = new vil_image_view<vxl_byte>(npixels_x,npixels_y,3);
    result = world->expected_image<APM_MOG_MC_3_3>(camera_metadata, expected_img, *mask_img, bin_index,scale_index);
  }
  else if (voxel_type == "apm_mog_mc_4_3") {
    expected_img = new vil_image_view<vxl_byte>(npixels_x,npixels_y,4);
    result = world->expected_image<APM_MOG_MC_4_3>(camera_metadata, expected_img, *mask_img, bin_index,scale_index);
  }
  else if (voxel_type == "edges") {
    bvxm_edge_ray_processor edge_proc(world);
    expected_img = new vil_image_view<vxl_byte>(npixels_x,npixels_y,1);
    result = edge_proc.expected_edge_image(camera_metadata,expected_img,1.0f,scale_index);
  }
  else if (voxel_type == "apm_float"){
    expected_img = new vil_image_view<float>(npixels_x,npixels_y,1);
    result = world->expected_image<FLOAT>(camera_metadata, expected_img, *mask_img, bin_index,scale_index);
  }
  else
    std::cerr << "Error in bvxm_render_expected_image_process: Unknown appearance model\n";

  //store output
  // process has 2 outputs:
  // output[0]: The rendered frame
  // output[1]: A confidence map, giving probability that each pixel was seen from the original view.
  unsigned j = 0;
  pro.set_output_val<vil_image_view_base_sptr>(j++,  expected_img);
  vil_image_view_base_sptr mask_sptr = mask_img;
  pro.set_output_val<vil_image_view_base_sptr>(j++,  mask_sptr);

  return result;
}
