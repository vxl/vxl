#include "bvxm_render_virtual_view_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bvxm_render_virtual_view_process::bvxm_render_virtual_view_process()
{
  // process takes 5 inputs: 
  //input[0]: The original frame
  //input[1]: The original camera
  //input[2]: The camera to render the virtual view from
  //input[3]: The voxel world
  //input[4]: The apperance model type. This input must be either apm_mog_grey or apm_mog_rgb
  //          any other string will initialize the value to apm_mog_grey
  //input[5]: The bin index to be updated
  input_data_.resize(6,brdb_value_sptr(0));
  input_types_.resize(6);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
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


bool bvxm_render_virtual_view_process::execute()
{

  // Sanity check
  if(!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<vil_image_view_base_sptr>* input0 = 
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr img_og = input0->value();

  brdb_value_t<vpgl_camera_double_sptr>* input1 = 
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());
  vpgl_camera_double_sptr camera_og = input1->value();

  brdb_value_t<vpgl_camera_double_sptr>* input2 = 
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[2].ptr());
  vpgl_camera_double_sptr camera_virtual = input2->value();

  brdb_value_t<bvxm_voxel_world_sptr>* input3 = 
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[3].ptr());
  bvxm_voxel_world_sptr world = input3->value();

  brdb_value_t<vcl_string>* input4 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[4].ptr());
  vcl_string voxel_type = input4->value();

  brdb_value_t<unsigned>* input5 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned bin_index = input5->value();

  //create original image metadata:
  bvxm_image_metadata obs_og(img_og,camera_og);
 
  // render view
  vil_image_view_base_sptr vis_prob = new vil_image_view<float>(img_og->ni(),img_og->nj(),1);
  // the world expects the virtual view to be of the same type as the input view.
  vil_image_view_base_sptr virtual_img;
  switch(img_og->pixel_format())
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
      vcl_cerr << "error: bvxm_render_virtual_view_process: unknown pixel format " << img_og->pixel_format() << vcl_endl;
      return false;
  }

  vil_image_view<float> *vis_prob_view = static_cast<vil_image_view<float>*>(vis_prob.ptr());
  
  bool result;
   if (voxel_type == "apm_mog_rgb"){
     if (img_og->nplanes() == 3)
      result = world->virtual_view<APM_MOG_RGB>(obs_og,camera_virtual,virtual_img,*vis_prob_view, bin_index);
     else {
       vcl_cerr<< "error: bvxm_render_virtual_view_process: voxel_type(apm_mog_rgb) does not match, input image" << vcl_endl;
       return false;
     }
   }
   else {
     result = world->virtual_view<APM_MOG_GREY>(obs_og,camera_virtual,virtual_img,*vis_prob_view, bin_index);
   }

  //store output
  brdb_value_sptr output0 = 
    new brdb_value_t<vil_image_view_base_sptr>(virtual_img);
  output_data_[0] = output0;

  brdb_value_sptr output1 = 
    new brdb_value_t<vil_image_view_base_sptr>(vis_prob);
  output_data_[1] = output1;

  return result;
}



