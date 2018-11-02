// This is brl/bseg/boxm/algo/rt/pro/processes/boxm_update_rt_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Process to update the scene from an image and camera pair
//
// \author Gamze Tunali
// \date Apr 21, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/rt/boxm_update_image_functor.h>
#include <boxm/boxm_apm_traits.h> // for BOXM_APM_MOG_GREY
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <boxm/sample/algo/boxm_mob_grey_processor.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <boxm/sample/boxm_sample.h>

namespace boxm_update_rt_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm_update_rt_process_cons(bprb_func_process& pro)
{
  using namespace boxm_update_rt_process_globals;

  // process takes 5 inputs and no output
  //input[0]: The observation image
  //input[1]: The camera of the observation
  //input[2]: The scene
  //input[3]: The bin index to be updated
  //input[4]: Use black background?
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "boxm_scene_base_sptr";
  input_types_[3] = "unsigned";
  input_types_[4] = "bool";

  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_update_rt_process(bprb_func_process& pro)
{
  using namespace boxm_update_rt_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << "boxm_update_rt_process: The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  vil_image_view_base_sptr input_image = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  /* unsigned bin_index = */   pro.get_input<unsigned>(i++);
  bool use_black_background =  pro.get_input<bool>(i++);

  // check the input validity
  if ((input_image == nullptr) || (camera == nullptr) || (scene == nullptr)) {
     std::cout << "boxm_update_rt_process: null input value, cannot run" << std::endl;
     return false;
  }

  // Mixture of Gaussians
  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    auto *img_byte
 = dynamic_cast<vil_image_view<vxl_byte>*>(input_image.ptr());
    vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> img(img_byte->ni(), img_byte->nj(), 1);
    vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
    if (!scene->multi_bin())
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_update_image_rt<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, camera,img, use_black_background);
    }
    else
    {
      std::cout<<"Not yet implemented"<<std::endl;
    }
  // Mixture of Beta's
  } else if (scene->appearence_model() == BOXM_APM_MOB_GREY) {
    auto *img_byte
 = dynamic_cast<vil_image_view<vxl_byte>*>(input_image.ptr());
    vil_image_view<boxm_apm_traits<BOXM_APM_MOB_GREY>::obs_datatype> img(img_byte->ni(), img_byte->nj(), 1);
    vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
    if (!scene->multi_bin())
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_update_image_rt<short, boxm_sample<BOXM_APM_MOB_GREY> >(*s, camera,img, use_black_background);
    }
    else
    {
      std::cout<<"Not yet implemented"<<std::endl;
    }
  } else if (scene->appearence_model() == BOXM_APM_SIMPLE_GREY) {
    auto *img_byte
 = dynamic_cast<vil_image_view<vxl_byte>*>(input_image.ptr());
    vil_image_view<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> img(img_byte->ni(), img_byte->nj(), 1);
    vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
    if (!scene->multi_bin())
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_update_image_rt(*s, camera,img, use_black_background);
    }
    else
    {
      std::cout<<"Not yet implemented"<<std::endl;
    }
  }
  else {
    std::cout << "boxm_update_rt_process: undefined APM type" << std::endl;
    return false;
  }

  return true;
}
