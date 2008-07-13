#include "bvxm_detect_multiscale_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_threshold.h>

#include <vpgl/vpgl_camera.h>

#include <multiscale/bvxm_multi_scale_voxel_world.h>
#include <multiscale/bvxm_multiscale_util.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <vpgl/vpgl_rational_camera.h>

bvxm_detect_multiscale_process::bvxm_detect_multiscale_process()
{
  //process takes 7 inputs
  //input[0]: The observation image
  //input[1]: The camera of the observation
  //input[2]: The voxel world
  //input[3]: The apperance model type :this input must be either apm_mog_grey or apm_mog_rgb
  //          any other string will initialize the value for apm_mog_grey
  //input[4]: The bin index to be updated
  //input[5]  The image scale
  //input[6]: maximum number of scales
  //input[7]: scale at which output is required

  input_data_.resize(8,brdb_value_sptr(0));
  input_types_.resize(8);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_multi_scale_voxel_world_sptr";
  input_types_[3] = "vcl_string";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "unsigned";
  input_types_[7] = "unsigned";


  //output vector has 2 outputs
  //output[0] : The updated probability map
  //output[1] : The mask of image pixels used in update
  output_data_.resize(3,brdb_value_sptr(0));
  output_types_.resize(3);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "vil_image_view_base_sptr";
  output_types_[2]= "vil_image_view_base_sptr";
}


bool bvxm_detect_multiscale_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr img = input0->value();

  brdb_value_t<vpgl_camera_double_sptr>* input1 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());
  vpgl_camera_double_sptr camera = input1->value();

  brdb_value_t<bvxm_multi_scale_voxel_world_sptr>* input2 =
    static_cast<brdb_value_t<bvxm_multi_scale_voxel_world_sptr>* >(input_data_[2].ptr());
  bvxm_multi_scale_voxel_world_sptr world = input2->value();

  brdb_value_t<vcl_string>* input3 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[3].ptr());
  vcl_string voxel_type = input3->value();

  brdb_value_t<unsigned>* input4 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned bin_index = input4->value();

  brdb_value_t<unsigned>* input5 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[5].ptr());
  unsigned curr_scale = input5->value();

  brdb_value_t<unsigned>* input6 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[6].ptr());
  unsigned max_scale = input6->value();

  brdb_value_t<unsigned>* input7 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[7].ptr());
  unsigned req_scale = input7->value();

  if (req_scale<curr_scale || req_scale>max_scale)
    return false;

  typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype obs_datatype;

  if (voxel_type == "apm_mog_rgb")
  typedef bvxm_voxel_traits<APM_MOG_RGB>::obs_datatype obs_datatype;

  vil_image_view<float> prob_map;
  vil_image_view<bool> mask;

  unsigned scale=curr_scale;
  while (scale!=req_scale)
  {
    img=bvxm_multiscale_util::downsample_image_by_two(img);
    scale++;
  }
  camera=bvxm_multiscale_util::downsample_camera( camera, req_scale);
  prob_map.set_size(img->ni(),img->nj());
  mask.set_size(img->ni(),img->nj());
  bool result;

  bvxm_image_metadata observation(img,camera);

  if (voxel_type == "apm_mog_rgb")
    result = world->pixel_probability_density<APM_MOG_RGB>(observation,prob_map, mask, bin_index,req_scale);
  else
    result = world->pixel_probability_density<APM_MOG_GREY>(observation,prob_map, mask, bin_index,req_scale);

  if (!result) {
    vcl_cerr << "error bvxm_detect_changes_process: failed to detect changes\n";
    return false;
  }
  float threshold_value = 0.5;
  float min_value = 0, max_value = 0;
  vil_math_value_range(prob_map, min_value, max_value);
  threshold_value = (min_value+max_value)/2;
  vil_image_view<bool> binary_img;
  vil_image_view<vxl_byte> gray_img;
  vil_threshold_above<float>(prob_map, binary_img, threshold_value);
  vcl_cout<<"min "<< min_value <<" max "<<max_value<< vcl_endl;
  vil_convert_stretch_range_limited<float>(prob_map,gray_img,min_value,max_value);
  //store output
  output_data_[0] = 
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(prob_map));

  output_data_[1] = 
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<bool>(mask));

  output_data_[2] = 
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(gray_img));

  return true;
}


vpgl_camera_double_sptr bvxm_detect_multiscale_process::downsample_camera_by_two(vpgl_camera_double_sptr camera)
{
  if ( vpgl_rational_camera<double>* rat_camera = dynamic_cast<vpgl_rational_camera<double>*> (camera.as_pointer()))
  {
    vpgl_rational_camera<double>* new_rat_camera=new vpgl_rational_camera<double>(*rat_camera);
    double u_s,v_s;
    rat_camera->image_scale(u_s,v_s);
    new_rat_camera->set_image_scale(u_s/2,v_s/2);

    double u_off,v_off;
    rat_camera->image_offset(u_off,v_off);
    new_rat_camera->set_image_offset(u_off/2,v_off/2);

    return new_rat_camera;
  }
  else
  {
    return NULL;
  }
}
