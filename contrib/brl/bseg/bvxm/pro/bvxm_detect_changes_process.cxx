#include "bvxm_detect_changes_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_threshold.h>

#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>


bvxm_detect_changes_process::bvxm_detect_changes_process()
{
  //process takes 4 inputs
  //input[0]: The observation to detect changes
  //input[1]: The camera of the observation
  //input[2]: The voxel world
  //input[3]: The apperance model type :this input must be either apm_mog_grey or apm_mog_rgb
  //          any other string will initialize the value for apm_mog_grey
  //input[4]: The bin index to be updated
  input_data_.resize(5,brdb_value_sptr(0));
  input_types_.resize(5);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_voxel_world_sptr";
  input_types_[3] = "vcl_string";
  input_types_[4] = "unsigned";

  //process has 1 output
  //output[0]: The change detection image
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
}


bool bvxm_detect_changes_process::execute()
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

  brdb_value_t<bvxm_voxel_world_sptr>* input2 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[2].ptr());
  bvxm_voxel_world_sptr world = input2->value();

  brdb_value_t<vcl_string>* input3 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[3].ptr());
  vcl_string voxel_type = input3->value();

  brdb_value_t<unsigned>* input4 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned bin_index = input4->value();

  //create metadata:
  bvxm_image_metadata observation(img,camera);

  //get probability density of all pixels in image
  vil_image_view<float> prob_map(img->ni(),img->nj(),1);
  vil_image_view<bool> mask(img->ni(),img->nj(),1);

  if (voxel_type == "apm_mog_rgb")
    world->pixel_probability_density<APM_MOG_RGB>(observation,prob_map,mask,bin_index);
  else
    world->pixel_probability_density<APM_MOG_GREY>(observation,prob_map,mask,bin_index);

  // TODO: filtering / thresholding if necessary (Thom?)

  // the following thresholding is added temporarily in order to produce an
  // image that can be saved threshold the image to make sure that it is binary
  float threshold_value = 0.5;
  float min_value = 0, max_value = 0;
  vil_math_value_range(prob_map, min_value, max_value);
  threshold_value = (min_value+max_value)/2;
  vil_image_view<bool> binary_img;
  vil_threshold_above<float>(prob_map, binary_img, threshold_value);

  //store output
  brdb_value_sptr output0 =
#if 0
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(prob_map));
#else
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<bool>(binary_img));
#endif
  output_data_[0] = output0;

  return true;
}

