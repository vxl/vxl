#include "brec_create_mog_image_process.h"

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_image_metadata.h>


brec_create_mog_image_process::brec_create_mog_image_process()
{
  //inputs
  input_data_.resize(5,brdb_value_sptr(0));
  input_types_.resize(5);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "bvxm_voxel_world_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "bbgm_image_sptr";  // output mog image as a voxel world with one slab

  //parameters()->add("use most probable mode to create mog", "most_prob", true);   // otherwise uses expected values of the mixtures at voxels along the path of the rays
  parameters()->add("method to use to create a mog image", "mog_method", (unsigned)bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE);   // otherwise uses expected values of the mixtures at voxels along the path of the rays
  parameters()->add("number of samples if using random sampling as mog_method", "n_samples", (unsigned)10);
  parameters()->add("ni for output mog", "ni", (unsigned)1000);  // should be set according to each test image
  parameters()->add("nj for output mog", "nj", (unsigned)1000);
  parameters()->add("nplanes for output expected view of output mog", "nplanes", (unsigned)1);
  parameters()->add("verbose", "verbose", false);
}

bool brec_create_mog_image_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs

  brdb_value_t<vpgl_camera_double_sptr>* input0 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[0].ptr());

  vpgl_camera_double_sptr camera = input0->value();

  brdb_value_t<bvxm_voxel_world_sptr>* input1 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[1].ptr());

  bvxm_voxel_world_sptr world = input1->value();

   brdb_value_t<vcl_string>* input2 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[2].ptr());

  vcl_string voxel_type = input2->value();

  brdb_value_t<unsigned>* input3 = static_cast<brdb_value_t<unsigned>* >(input_data_[3].ptr());
  unsigned bin_index = input3->value();

  brdb_value_t<unsigned>* input4 = static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned scale_index = input4->value();

  bool verbose = false;
  parameters()->get_value("verbose", verbose);

  //set class variables
  parameters()->get_value("ni", ni_);
  parameters()->get_value("nj", nj_);
  parameters()->get_value("nplanes", nplanes_);

  // if the world is not updated yet, we just return the input image
  unsigned num_observations = 0;
  if (voxel_type == "apm_mog_grey")
    num_observations = world->num_observations<APM_MOG_GREY>(bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_3_3")
    num_observations = world->num_observations<APM_MOG_MC_3_3>(bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_4_3")
    num_observations = world->num_observations<APM_MOG_MC_4_3>(bin_index,scale_index);
  else {
    vcl_cout << "In brec_create_mog_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (num_observations == 0)
  {
    vcl_cout << "In brec_create_mog_image_process::execute() -- input voxel world has 0 observations, not possible to create MOG image!\n";
    return false;
  }

  bool done = false;
  if (voxel_type == "apm_mog_grey")
    done = this->create_mog<APM_MOG_GREY>(camera,world,bin_index,scale_index,verbose);
  else if (voxel_type == "apm_mog_mc_3_3")
    done = this->create_mog<APM_MOG_MC_3_3>(camera,world,bin_index,scale_index,verbose);
  else if (voxel_type == "apm_mog_mc_4_3")
    done = this->create_mog<APM_MOG_MC_4_3>(camera,world,bin_index,scale_index,verbose);
  else {
    vcl_cout << "In brec_create_mog_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (!done) {
    vcl_cout << "In brec_create_mog_image_process::execute() - problems in creating mixture of gaussian image!\n";
    return false;
  }

  // turn the bvxm_slab into a bbgm_image_of instance for binary io (i.e. use processes in bbgm_pro)
  done = false;
  if (voxel_type == "apm_mog_grey")
    done = this->create_bbgm_image<APM_MOG_GREY>(verbose);
  else if (voxel_type == "apm_mog_mc_3_3")
    done = this->create_bbgm_image<APM_MOG_MC_3_3>(verbose);
  else if (voxel_type == "apm_mog_mc_4_3")
    done = this->create_bbgm_image<APM_MOG_MC_4_3>(verbose);
  else {
    vcl_cout << "In brec_create_mog_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (!done) {
    vcl_cout << "In brec_create_mog_image_process::execute() - problems in creating mixture of gaussian image!\n";
    return false;
  }

  // return the output img
  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(out_img_);
  output_data_[0] = output0;

  brdb_value_sptr output1 = new brdb_value_t<bbgm_image_sptr>(out_model_img_);
  output_data_[1] = output1;

  return true;
}

