#include "bvxm_normalize_image_process.h"

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_image_metadata.h>


bvxm_normalize_image_process::bvxm_normalize_image_process()
{

  //inputs
  input_data_.resize(6,brdb_value_sptr(0));
  input_types_.resize(6);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_voxel_world_sptr";
  input_types_[3] = "vcl_string"; 
  input_types_[4] = "unsigned";  
  input_types_[5] = "unsigned"; 

  //output
  output_data_.resize(3,brdb_value_sptr(0));
  output_types_.resize(3);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "float";  // output a
  output_types_[2]= "float";  // output b

  parameters()->add("use most probable mode to create mog", "most_prob", true);   // otherwise uses expected values of the mixtures at voxels along the path of the rays 
  parameters()->add("start value for a", "a_start", 0.6f);
  parameters()->add("increment for a", "a_inc", 0.05f);
  parameters()->add("end value for a", "a_end", 1.8f);
  // The following defaults are carried from Thom's code.
  // However here we always work with images scaled to [0,1] interval.
  // Thom had assumed byte images, so the defaults are divided by 255 for b.
  parameters()->add("start value for intervals of b", "b_start", (100.0f/255.0f));  // start search in [-200, 200] with 100 increments for a byte image
  parameters()->add("end value for intervals of b", "b_end", (5.0f/255.0f));        // end search in [-10, 10] with 5 increments
  parameters()->add("ratio that determines increment value for intervals of b", "b_ratio", 0.5f);           // half the intervals, e.g. if start with 100, then 50, 25, 12, etc.
  parameters()->add("verbose", "verbose", false);
}

bool bvxm_normalize_image_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());

  vil_image_view_base_sptr input_img = input0->value();

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

    brdb_value_t<unsigned>* input5 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[5].ptr());
  unsigned scale_index = input5->value();
  bool verbose = false;
  parameters()->get_value("verbose", verbose);

  //set class variables
  ni_ = input_img->ni();
  nj_ = input_img->nj();
  nplanes_ = input_img->nplanes();

  // if the world is not updated yet, we just return the input image
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

  if(num_observations == 0)
  {
      //return the input img.Thus, a=1, b=0
      brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(input_img);
      output_data_[0] = output0;
      brdb_value_sptr output1 = new brdb_value_t<float>(1.0f); // a
      brdb_value_sptr output2 = new brdb_value_t<float>(0.0f); // b
      output_data_[1] = output1;
      output_data_[2] = output2;  
      if (verbose)
      {
          vcl_ofstream file;
          file.open("./normalization_parameters.txt", vcl_ofstream::app);
          file << 1.0 << ' ' << 0.0 <<'\n';
          file.close();
      }
      return true;
 }

  vil_image_view<float>*  input_img_float_stretched = new vil_image_view<float>( ni_, nj_, nplanes_ );
  //calculate a, b parameters
  float a = 1.0;
  float b = 0.0;  
  if (voxel_type == "apm_mog_grey") 
    this->norm_parameters<APM_MOG_GREY>(input_img,input_img_float_stretched,camera,world,bin_index,scale_index,verbose,a,b);
  else if (voxel_type == "apm_mog_mc_3_3")
    this->norm_parameters<APM_MOG_MC_3_3>(input_img,input_img_float_stretched,camera,world,bin_index,scale_index,verbose,a,b);
  else if (voxel_type == "apm_mog_mc_4_3")
    this->norm_parameters<APM_MOG_MC_4_3>(input_img,input_img_float_stretched,camera,world,bin_index,scale_index,verbose,a,b);
  else{
    vcl_cout << "In bvxm_normalize_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }
  
  // Normalize the image with the best a and b.
  //vil_image_view<vxl_byte> output_img(ni, nj, nplanes);
  //normalize_image<vxl_byte>(*input_image_sptr, output_img, a, b, 255);

  vil_image_view<float> output_img_float(ni_, nj_, nplanes_);
  normalize_image(*input_img_float_stretched, output_img_float, a, b, 1.0f);

  vil_image_view<float> output_img_stretched(ni_, nj_, nplanes_);
  vil_convert_stretch_range_limited<float>(output_img_float, output_img_stretched, 0.0f, 1.0f, 0.0f, 255.0f);
  vil_image_view<vxl_byte> output_img;
  vil_convert_cast(output_img_stretched, output_img);

  // return the output img
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(output_img));
  output_data_[0] = output0;

  brdb_value_sptr output1 = new brdb_value_t<float>(a);
  brdb_value_sptr output2 = new brdb_value_t<float>(b*255.0f); // switch back to byte
  output_data_[1] = output1;
  output_data_[2] = output2;

  return true;
}

