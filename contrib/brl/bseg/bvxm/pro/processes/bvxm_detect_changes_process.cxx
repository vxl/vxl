//This is brl/bseg/bvxm/pro/processes/bvxm_detect_changes_process.cxx
//:
// \file
// \brief A class for detecting changes using a voxel world .
//
// \author Daniel Crispell
// \date 02/10/2008
// \verbatim
//  Modifications
//   Isabel Restrepo - 1/27/09 - converted process-class to functions which is the new design for bvxm_processes.
// \endverbatim


#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_threshold.h>//   <none yet>//   <none yet>

#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bool bvxm_detect_changes_process(bprb_func_process& pro)
{
  //inputs
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
  //input[5]: The image scale index detected
  unsigned n_inputs_ = 6;
  if (pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string voxel_type = pro.get_input<vcl_string>(i++);
  unsigned bin_index = pro.get_input<unsigned>(i++);
  unsigned scale_index = pro.get_input<unsigned>(i++);

  //check input validity
  if (!img) {
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
  if (!camera) {
    vcl_cout << pro.name() <<" :--  Input 1 is not valid!\n";
    return false;
  }
  if (!world) {
    vcl_cout << pro.name() <<" :--  Input 2 is not valid!\n";
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
      vcl_cerr << "Error bvxm_detect_changes_process: appereance model type" << voxel_type << "does not support images with "
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
      vcl_cerr << "Error bvxm_detect_changes_process: appereance model type" << voxel_type << "does not support images with "
               << observation.img->nplanes()<< " planes\n";
      return false;
    }
    result = world->pixel_probability_density<APM_MOG_MC_4_3>(observation,prob_map, mask, bin_index,scale_index);
  }
  else
    vcl_cerr << "Error in: bvxm_detect_changes_processor: Unsuppported appereance model\n";

  if (!result) {
    vcl_cerr << "Error bvxm_detect_changes_process: failed to detect changes\n";
    return false;
  }
  // TODO: filtering / thresholding if necessary (Thom?)

  // the following thresholding is added temporarily in order to produce an
  // image that can be saved threshold the image to make sure that it is binary
  float threshold_value = 0.5;
  float min_value = 0, max_value = 0;
  vil_math_value_range(prob_map, min_value, max_value);

  threshold_value = (min_value+max_value)/2;
  vil_image_view<bool> binary_img;
  vil_threshold_above<float>(prob_map, binary_img, threshold_value);

  //set and store output
  unsigned j=0;
  vcl_vector<vcl_string> output_types_(3);
  //the probability map
  output_types_[j++] = "vil_image_view_base_sptr";
  //the mask
  output_types_[j++] = "vil_image_view_base_sptr";
  //the binary image
  output_types_[j++] = "vil_image_view_base_sptr";
  pro.set_output_types(output_types_);

  j = 0;
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(prob_map));
  pro.set_output(j++, output0);

  brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<bool>(mask));
  pro.set_output(j++,output1);

  brdb_value_sptr output2 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<bool>(binary_img));
  pro.set_output(j++,output2);

  return true;
}

