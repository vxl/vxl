//This is brl/bseg/bvxm/pro/processes/bvxm_create_mog_image_process.cxx
#include "bvxm_create_mog_image_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <bvxm/pro/processes/bvxm_normalization_util.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>

#include <brdb/brdb_value.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera_double_sptr.h>

bool bvxm_create_mog_image_process_cons(bprb_func_process& pro)
{
  //inputs

  //0: The voxel world
  //1: The appearance model type
  //2: The illumination bin index nplanes_
  //3: The scale
  //4: The camera
  //5: ni, the size of the output MOG image
  //6: nj
  vcl_vector<vcl_string> input_types_;

  input_types_.push_back("bvxm_voxel_world_sptr");
  input_types_.push_back("vcl_string");
  input_types_.push_back("unsigned");
  input_types_.push_back("unsigned");
  input_types_.push_back("vpgl_camera_double_sptr");
  input_types_.push_back("unsigned");
  input_types_.push_back("unsigned");

  if (!pro.set_input_types(input_types_))
    return false;

  //output
  vcl_vector<vcl_string> output_types_;
  output_types_.push_back("bvxm_voxel_slab_base_sptr");
  return pro.set_output_types(output_types_);
}

bool bvxm_create_mog_image_process(bprb_func_process& pro)
{
   //get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string voxel_type = pro.get_input<vcl_string>(i++);
  unsigned bin_index = pro.get_input<unsigned>(i++);;
  unsigned scale_index = pro.get_input<unsigned>(i++);

  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);

  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);

  if (!camera) {
    vcl_cout << pro.name() <<" :--  Input 1  is not valid!\n";
    return false;
  }
  if (!world) {
    vcl_cout << pro.name() <<" :--  Input 2  is not valid!\n";
    return false;
  }

  //get parameters and overwrite global values
  vcl_string param_mog_method_ = "mog_method";
  const vcl_string param_nsamples_ = "n_samples";

  unsigned n_samples = 10;
  unsigned mog_creation_method_ = bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE;

  //: assumes an xml file sets these values if defaults won't be used
  pro.parameters()->get_value(param_mog_method_, mog_creation_method_);
  pro.parameters()->get_value(param_nsamples_, n_samples);
  bool verbose_ = false;
  pro.parameters()->get_value("verbose", verbose_);

  //: the image in the observation is only used to determine \p ni and \p nj for the output MOG.
  //  so create a dummy image
  vil_image_view_base_sptr dummy_img = new vil_image_view<vxl_byte>(ni, nj, 1);
  bvxm_image_metadata observation(dummy_img,camera);

  // if the world is not updated yet, we just return 0 as voxel slab base (the processes which use this slab should check whether its zero
  unsigned num_observations = 0;
  if (voxel_type == "apm_mog_grey")
    num_observations = world->num_observations<APM_MOG_GREY>(bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_3_3")
    num_observations = world->num_observations<APM_MOG_MC_3_3>(bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_4_3")
    num_observations = world->num_observations<APM_MOG_MC_4_3>(bin_index,scale_index);
  else{
    vcl_cout << "In bvxm_normalize_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  bvxm_voxel_slab_base_sptr mog_image;

  if (num_observations == 0)
  {
    pro.set_output_val<bvxm_voxel_slab_base_sptr>(0,mog_image); // returns empty pointer
    return true;
  }

  if (verbose_) {
    switch (mog_creation_method_) {
      case bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE:
      { vcl_cout << "using most probable modes' colors to create mog image "; } break;
      case bvxm_mog_image_creation_methods::EXPECTED_VALUE:
      { vcl_cout << "using expected colors to create mog image "; } break;
      case bvxm_mog_image_creation_methods::SAMPLING:
      { vcl_cout << "using random sampling to create mog image "; } break;
      default:
      { vcl_cout << "In bvxm_normalize_image_process::norm_parameters() - unrecognized option: " << mog_creation_method_ << " to create mog image\n"; return false; }
    }
  }

  bool good = false;
  if (voxel_type == "apm_mog_grey")
    good = mix_gaussian<APM_MOG_GREY>(world,mog_creation_method_,bin_index,scale_index,n_samples,observation,mog_image);
  else if (voxel_type == "apm_mog_mc_3_3")
    good = mix_gaussian<APM_MOG_MC_3_3>(world,mog_creation_method_,bin_index,scale_index,n_samples,observation,mog_image);
  else if (voxel_type == "apm_mog_mc_4_3")
    good = mix_gaussian<APM_MOG_MC_4_3>(world,mog_creation_method_, bin_index,scale_index,n_samples,observation,mog_image);
  else{
    vcl_cout << "In bvxm_normalize_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (!good)
    return false;

  unsigned j=0;
  pro.set_output_val<bvxm_voxel_slab_base_sptr>(j++,mog_image);
  return true;
}
