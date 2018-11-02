// This is brl/bseg/boxm/algo/rt/pro/processes/boxm_generate_opt3_samples_process.cxx
//:
// \file
// \brief Process to generate samples needed for boxm_opt3_optimizer_process from a given image/camera pair
//
// \author Daniel Crispell
// \date Feb 17, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/rt/boxm_generate_opt3_sample_functor.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <boxm/sample/boxm_sample.h>

namespace boxm_generate_opt3_samples_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm_generate_opt3_samples_process_cons(bprb_func_process& pro)
{
  using namespace boxm_generate_opt3_samples_process_globals;

  // process takes 5 inputs and no output
  //input[0]: The observation image
  //input[1]: The camera of the observation
  //input[2]: The scene
  //input[3]: image name for saving scene
  //input[4]: use black background
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "boxm_scene_base_sptr";
  input_types_[3] = "vcl_string";
  input_types_[4] = "bool";

  std::vector<std::string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_generate_opt3_samples_process(bprb_func_process& pro)
{
  using namespace boxm_generate_opt3_samples_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << "boxm_generate_opt3_samples_process: The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  vil_image_view_base_sptr input_image = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  std::string img_name =  pro.get_input<std::string>(i++); // TODO - unused!!
  bool use_black_background =  pro.get_input<bool>(i++);

  // check the input validity
  if ((input_image == nullptr) || (camera == nullptr) || (scene == nullptr)) {
    std::cout << "boxm_generate_opt3_samples_process: null input value, cannot run" << std::endl;
    return false;
  }

  switch (scene->appearence_model())
  {
   case BOXM_APM_SIMPLE_GREY:
    {
      auto *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(input_image.ptr());
      vil_image_view<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> img(img_byte->ni(), img_byte->nj(), 1);
      vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      if (!scene->multi_bin())
      {
        typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
        auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
        boxm_generate_opt3_sample<short, boxm_sample<BOXM_APM_SIMPLE_GREY>, BOXM_AUX_OPT3_MOG_GREY>(*s, camera,img, img_name, use_black_background);
      }
      break;
    }
   case BOXM_APM_MOG_GREY:
    {
      auto *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(input_image.ptr());
      vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> img(img_byte->ni(), img_byte->nj(), 1);
      vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      if (!scene->multi_bin())
      {
        typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
        auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
        boxm_generate_opt3_sample<short, boxm_sample<BOXM_APM_MOG_GREY>, BOXM_AUX_OPT3_MOG_GREY>(*s, camera,img, img_name, use_black_background);
      }
      break;
    }
   default:
    std::cout << "boxm_generate_opt3_samples_process: unsupported APM type" << std::endl;
    return false;
  }

  return true;
}
