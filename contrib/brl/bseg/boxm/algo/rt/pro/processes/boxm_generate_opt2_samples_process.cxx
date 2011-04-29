// This is brl/bseg/boxm/algo/rt/pro/processes/boxm_generate_opt2_samples_process.cxx
//:
// \file
// \brief Process to generate samples needed for boxm_opt2_optimizer_process from a given image/camera pair
//
// \author Daniel Crispell
// \date Feb 17, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/rt/boxm_generate_opt2_sample_functor.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <boxm/sample/boxm_sample.h>

namespace boxm_generate_opt2_samples_process_globals
{
  const unsigned n_inputs_ = 7;
  const unsigned n_outputs_ = 0;
}

bool boxm_generate_opt2_samples_process_cons(bprb_func_process& pro)
{
  using namespace boxm_generate_opt2_samples_process_globals;

  // process takes 7 inputs and no output
  //input[0]: The observation image
  //input[1]: The camera of the observation
  //input[2]: The scene
  //input[3]: image name for saving scene
  //input[4]: shadow prior
  //input[5]: shadow sigma
  //input[6]: use black background
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "boxm_scene_base_sptr";
  input_types_[3] = "vcl_string";
  input_types_[4] = "float";
  input_types_[5] = "float";
  input_types_[6] = "bool";

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_generate_opt2_samples_process(bprb_func_process& pro)
{
  using namespace boxm_generate_opt2_samples_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << "boxm_generate_opt2_samples_process: The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  vil_image_view_base_sptr input_image = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(1);
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(2);
  vcl_string img_name =  pro.get_input<vcl_string>(3); // TODO - unused!!
  float shadow_prior = pro.get_input<float>(4);
  float shadow_sigma = pro.get_input<float>(5);
  bool use_black_background =  pro.get_input<bool>(6);

  // check the input validity
  if ((input_image == 0) || (camera == 0) || (scene == 0)) {
    vcl_cout << "boxm_generate_opt2_samples_process: null input value, cannot run" << vcl_endl;
    return false;
  }

  switch (scene->appearence_model())
  {
   case BOXM_APM_SIMPLE_GREY:
    {
      vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(input_image.ptr());
      vil_image_view<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> img(img_byte->ni(), img_byte->nj(), 1);
      vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      // create alternate appearance models
      vcl_vector<float> alt_appearance_priors;
      vcl_vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype> alt_appearance_models;
      if (shadow_prior > 0.0f) {
        boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype shadow_model(0.0f, shadow_sigma);
        alt_appearance_priors.push_back(shadow_prior);
        alt_appearance_models.push_back(shadow_model);
      }
      if (!scene->multi_bin()) {
        typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
        boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
        boxm_generate_opt2_samples<short, boxm_sample<BOXM_APM_SIMPLE_GREY>, BOXM_AUX_OPT2_GREY >(*s, camera, img, img_name, alt_appearance_priors, alt_appearance_models, use_black_background);
      }
      else {
        vcl_cerr << "error: multi-bin scenes not supported\n";
      }
      break;
    }
   case BOXM_APM_MOG_GREY:
    {
      vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(input_image.ptr());
      vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> img(img_byte->ni(), img_byte->nj(), 1);
      vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      // create alternate appearance models
      vcl_vector<float> alt_appearance_priors;
      vcl_vector<boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype> alt_appearance_models;
      if (shadow_prior > 0.0f) {
        boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype shadow_model;
        // insert a single mode with mean at 0
        bsta_gauss_sf1 single_mode(0.0f, shadow_sigma*shadow_sigma);
        shadow_model.insert(bsta_num_obs<bsta_gauss_sf1>(single_mode), 1.0f);
        alt_appearance_priors.push_back(shadow_prior);
        alt_appearance_models.push_back(shadow_model);
      }
      if (!scene->multi_bin()) {
        typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
        boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
        boxm_generate_opt2_samples<short, boxm_sample<BOXM_APM_MOG_GREY>, BOXM_AUX_OPT2_GREY >(*s, camera, img, img_name, alt_appearance_priors, alt_appearance_models, use_black_background);
      }
      else {
        vcl_cerr << "error: multi-bin scenes not supported\n";
      }
      break;
    }
   default:
    vcl_cout << "boxm_generate_opt2_samples_process: unsupported APM type" << vcl_endl;
    return false;
  }

  return true;
}
