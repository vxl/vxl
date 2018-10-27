//This is brl/bseg/bvxm/pro/processes/bvxm_detect_changes_process.cxx
#include "bvxm_detect_changes_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_threshold.h>

#include <vpgl/vpgl_camera_double_sptr.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

//: initialize input and output types
bool bvxm_detect_changes_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_detect_changes_process_globals;
  //process takes 6 inputs
  //input[0]: The observation to detect changes
  //input[1]: The camera of the observation
  //input[2]: The voxel world
  //input[3]: The appearance model type, the supported strings are:
  //          -apm_mog_grey
  //          -apm_mog_rgb
  //          -apm_mog_mc_2_3
  //          -apm_mog_mc_3_3
  //          -apm_mog_mc_4_3
  //input[4]: The bin index to be updated
  //input[5]: The image scale index  detected
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_voxel_world_sptr";
  input_types_[3] = "vcl_string";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  //output has 2 outputs
  //output[0] : The updated probability map
  //output[1] : The mask of image pixels used in update
  //output[2] : Thresholded image: Binary
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "vil_image_view_base_sptr";
  output_types_[2]= "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//: detec changes
bool bvxm_detect_changes_process(bprb_func_process& pro)
{
  using namespace bvxm_detect_changes_process_globals;
  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  std::string voxel_type = pro.get_input<std::string>(i++);
  auto bin_index = pro.get_input<unsigned>(i++);
  auto scale_index = pro.get_input<unsigned>(i++);

  //check input validity
  if (!img) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
  if (!camera) {
    std::cout << pro.name() <<" :--  Input 1 is not valid!\n";
    return false;
  }
  if (!world) {
    std::cout << pro.name() <<" :--  Input 2 is not valid!\n";
    return false;
  }

  //create metadata:
  bvxm_image_metadata observation(img,camera);

  //get probability density of all pixels in image
  vil_image_view<float> prob_map(img->ni(),img->nj(),1);
  vil_image_view<bool> mask(img->ni(),img->nj(),1);

  bool result = true;

  if (voxel_type == "apm_mog_grey")
    result = world->pixel_probability_density<APM_MOG_GREY>(observation,prob_map, mask, bin_index,scale_index);
  else if (voxel_type == "apm_mog_rgb")
    result = world->pixel_probability_density<APM_MOG_RGB>(observation,prob_map, mask, bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_2_3")
  {
    if (observation.img->nplanes()!= 2)
    {
      std::cerr << "Error bvxm_detect_changes_process: appearance model type" << voxel_type << "does not support images with "
               << observation.img->nplanes()<< " planes\n";
      return false;
    }
    result = world->pixel_probability_density<APM_MOG_MC_2_3>(observation,prob_map, mask, bin_index,scale_index);
  }
  else if (voxel_type == "apm_mog_mc_3_3")
    result = world->pixel_probability_density<APM_MOG_MC_3_3>(observation,prob_map, mask, bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_4_3")
  {
    if (observation.img->nplanes()!= 4)
    {
      std::cerr << "Error bvxm_detect_changes_process: appearance model type" << voxel_type << "does not support images with "
               << observation.img->nplanes()<< " planes\n";
      return false;
    }
    result = world->pixel_probability_density<APM_MOG_MC_4_3>(observation,prob_map, mask, bin_index,scale_index);
  }
  else
    std::cerr << "Error in: bvxm_detect_changes_processor: Unsuppported appearance model\n";

  if (!result) {
    std::cerr << "Error bvxm_detect_changes_process: failed to detect changes\n";
    return false;
  }
  // TODO: filtering / thresholding iset and f necessary (Thom?)

  // the following thresholding is added temporarily in order to produce an
  // image that can be saved threshold the image to make sure that it is binary
  float threshold_value = 0.5;
  float min_value = 0, max_value = 0;
  vil_math_value_range(prob_map, min_value, max_value);

  threshold_value = (min_value+max_value)/2;
  vil_image_view<bool> binary_img;
  vil_threshold_above<float>(prob_map, binary_img, threshold_value);

  //store output
  unsigned j=0;
  //the probability map
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<float>(prob_map));
  //the mask
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<bool>(mask));
  //the binary image
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<bool>(binary_img));

  return true;
}
