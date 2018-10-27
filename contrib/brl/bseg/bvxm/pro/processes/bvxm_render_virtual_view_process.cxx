// This is brl/bseg/bvxm/pro/processes/bvxm_render_virtual_view_process.cxx
#include "bvxm_render_virtual_view_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vpgl/vpgl_camera_double_sptr.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

//: set input and output types
bool bvxm_render_virtual_view_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_render_virtual_view_process_globals;
  // process takes 6 inputs:
  //input[0]: The original frame
  //input[1]: The original camera
  //input[2]: The camera to render the virtual view from
  //input[3]: The voxel world
  //input[4]: The appearance model type. This input must be either apm_mog_grey or apm_mog_rgb
  //          any other string will initialize the value to apm_mog_grey
  //input[5]: The bin index to be updated
  //input[6]: The scale index to be rendered

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";
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
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//: renders a video frame from a new viewpoint (used for 3-D registration)
bool bvxm_render_virtual_view_process(bprb_func_process& pro)
{
  using namespace bvxm_render_virtual_view_process_globals;

  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr img_og = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera_og = pro.get_input< vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr camera_virtual = pro.get_input< vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input< bvxm_voxel_world_sptr>(i++);
  std::string voxel_type = pro.get_input<std::string>(i++);
  auto bin_index = pro.get_input<unsigned>(i++);
  auto scale_index =  pro.get_input<unsigned>(i++);

    //check input validity
  if (!img_og) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
  if (!camera_og) {
    std::cout << pro.name() <<" :--  Input 1  is not valid!\n";
    return false;
  }
  if (!camera_virtual) {
    std::cout << pro.name() <<" :--  Input 2  is not valid!\n";
    return false;
  }
  if (!world) {
    std::cout << pro.name() <<" :--  Input 3  is not valid!\n";
    return false;
  }

  //create original image metadata:
  bvxm_image_metadata obs_og(img_og,camera_og);

  // render view
  vil_image_view_base_sptr vis_prob = new vil_image_view<float>(img_og->ni(),img_og->nj(),1);
  // the world expects the virtual view to be of the same type as the input view.
  vil_image_view_base_sptr virtual_img;
  switch (img_og->pixel_format())
  {
  case VIL_PIXEL_FORMAT_BYTE:
    virtual_img = new vil_image_view<vxl_byte>(img_og->ni(),img_og->nj(),img_og->nplanes());
    break;
  case VIL_PIXEL_FORMAT_FLOAT:
    virtual_img = new vil_image_view<float>(img_og->ni(),img_og->nj(),img_og->nplanes());
    break;
  case VIL_PIXEL_FORMAT_RGB_BYTE:
    virtual_img = new vil_image_view<vil_rgb<unsigned char> >(img_og->ni(),img_og->nj(),img_og->nplanes());
    break;
  default:
    std::cerr << "error: bvxm_render_virtual_view_process: unknown pixel format " << img_og->pixel_format() << std::endl;
    return false;
  }

  auto *vis_prob_view = static_cast<vil_image_view<float>*>(vis_prob.ptr());

  bool result = true;

  if (voxel_type == "apm_mog_grey")
    result = world->virtual_view<APM_MOG_GREY>(obs_og,camera_virtual,virtual_img,*vis_prob_view, bin_index,scale_index);
  else if (voxel_type == "apm_mog_rgb") {
    if (img_og->nplanes() == 3)
      result = world->virtual_view<APM_MOG_RGB>(obs_og,camera_virtual,virtual_img,*vis_prob_view, bin_index,scale_index);
    else {
      std::cerr<< "error: bvxm_render_virtual_view_process: voxel_type(apm_mog_rgb) does not match, input image\n";
      return false;
    }
  }
  else if (voxel_type == "apm_mog_mc_2_3")
  {
    if (img_og->nplanes() == 2)
      result = world->virtual_view<APM_MOG_MC_2_3>(obs_og,camera_virtual,virtual_img,*vis_prob_view, bin_index,scale_index);
    else {
      std::cerr<< "error: bvxm_render_virtual_view_process: voxel_type(apm_mog_mc_2_3) does not match, input image\n";
      return false;
    }
  }
  else if (voxel_type == "apm_mog_mc_3_3")
  {
    if (img_og->nplanes() == 3)
      result = world->virtual_view<APM_MOG_MC_3_3>(obs_og,camera_virtual,virtual_img,*vis_prob_view, bin_index,scale_index);
    else {
      std::cerr<< "error: bvxm_render_virtual_view_process: voxel_type(apm_mog_mc_3_3) does not match, input image\n";
      return false;
    }
  }
  else if (voxel_type == "apm_mog_mc_4_3")
  {
    if (img_og->nplanes() == 4)
      result = world->virtual_view<APM_MOG_MC_4_3>(obs_og,camera_virtual,virtual_img,*vis_prob_view, bin_index,scale_index);
    else {
      std::cerr<< "error: bvxm_render_virtual_view_process: voxel_type(apm_mog_mc_4_3) does not match, input image\n";
      return false;
    }
  }
  else
    std::cerr << "Error in bvxm_render_virtual_view_process: Unknown appearance model\n";

  //store output

  pro.set_output_val<vil_image_view_base_sptr>(0, virtual_img);
  pro.set_output_val<vil_image_view_base_sptr>(1, vis_prob);

  return result;
}
