// This is brl/bseg/boxm/algo/rt/pro/processes/boxm_change_probabilty_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for obtaining expected depth map for the world
//
// \author Vishal Jain
// \date Aug 26, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/rt/boxm_change_probability_functor.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

namespace boxm_change_probability_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 2;
}

bool boxm_change_probability_process_cons(bprb_func_process& pro)
{
  using namespace boxm_change_probability_process_globals;

  //process takes 4 inputs
  //input[0]: scene binary file
  //input[1]: camera
  //input[2]: ni of the expected image
  //input[3]: nj of the expected image
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  //input_types_[3] = "unsigned";
  //input_types_[4] = "unsigned";

  // process has 2 outputs:
  // output[0]: rendered image
  // output[0]: mask
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_change_probability_process(bprb_func_process& pro)
{
  using namespace boxm_change_probability_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  vil_image_view_base_sptr input_img_ptr=pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);

  unsigned ni=input_img_ptr->ni();
  unsigned nj=input_img_ptr->nj();
  vil_image_view_base_sptr img;
  vil_image_view_base_sptr img_mask;


  // check the scene's app model
  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY) {
    vil_image_view<float> mask(ni,nj,1);
    vil_image_view<float> expected(ni,nj,1);

    auto *img_byte
 = dynamic_cast<vil_image_view<vxl_byte>*>(input_img_ptr.ptr());
    vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> img_in(img_byte->ni(), img_byte->nj(), 1);
    vil_convert_stretch_range_limited(*img_byte ,img_in, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);

    typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > type;
    auto* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
    boxm_change_prob_rt<short, boxm_sample<BOXM_APM_MOG_GREY> >(*scene,img_in, camera, expected, mask);
    img_mask = new vil_image_view<float>(mask);
    img = new vil_image_view<float>(expected);
  }
  else if (scene_ptr->appearence_model() == BOXM_APM_SIMPLE_GREY) {
    vil_image_view<float> mask(ni,nj,1);
    vil_image_view<float> expected(ni,nj,1);
    auto *img_byte
 = dynamic_cast<vil_image_view<vxl_byte>*>(input_img_ptr.ptr());
    vil_image_view<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> img_in(img_byte->ni(), img_byte->nj(), 1);
    vil_convert_stretch_range_limited(*img_byte ,img_in, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);

    typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > type;
    auto* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
    boxm_change_prob_rt<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >(*scene,img_in, camera, expected, mask);
    img_mask = new vil_image_view<float>(mask);
    img = new vil_image_view<float>(expected);
  }
  else {
    std::cout << "boxm_render_expected_process: undefined APM type" << std::endl;
    return false;
  }

  unsigned j = 0;
  pro.set_output_val<vil_image_view_base_sptr>(j++, img);
  pro.set_output_val<vil_image_view_base_sptr>(j++, img_mask);
  return true;
}
