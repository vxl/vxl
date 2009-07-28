// This is brl/bseg/boxm/pro/processes/boxm_render_expected_process.cxx
//:
// \file
// \brief A class for obtaining roc curve from change detection results.
//
// \author Gamze Tunali
// \date Apr 14, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_render_image.h>
#include <boxm/boxm_render_image_triangle.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_mog_grey_processor.h>
#include <boxm/boxm_sample_multi_bin.h>
#include <vil/vil_convert.h>

namespace boxm_render_expected_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 2;
}

bool boxm_render_expected_process_cons(bprb_func_process& pro)
{
  using namespace boxm_render_expected_process_globals;
  //process takes 4 inputs
  //input[0]: scene binary file
  //input[1]: camera
  //input[2]: ni of the expected image
  //input[3]: nj of the expected image
  //input[4]: bin number
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 output:
  // output[0]: rendered image
  // output[0]: mask
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

bool boxm_render_expected_process(bprb_func_process& pro)
{
  using namespace boxm_render_expected_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);

  vil_image_view_base_sptr img;
  vil_image_view_base_sptr img_mask;

  // check the scene's app model
  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY) {
    vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> expected(ni,nj);
    vil_image_view<float> mask(ni,nj);
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
      boxm_render_image_splatting_triangle<short, boxm_sample<BOXM_APM_MOG_GREY> >(*scene, camera, expected, mask,-1,true);
    }
    else
    {
      unsigned bin = pro.get_input<unsigned>(i++);
      vcl_cout<<"Multi Bin"<<vcl_endl;
      typedef boct_tree<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
      boxm_render_image_splatting_triangle<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(*scene, camera, expected, mask,bin,true);
    }

    //img = new vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype>(expected);
    img_mask = new vil_image_view<float>(mask);

    vil_image_view<unsigned char> *expected_byte = new vil_image_view<unsigned char>(expected.ni(),expected.nj(),expected.nplanes());
    vil_convert_stretch_range_limited(expected,*expected_byte, 0.0f, 1.0f);
    img = expected_byte;
  }
  else {
    vcl_cout << "boxm_render_expected_process: undefined APM type" << vcl_endl;
    return false;
  }

  unsigned j = 0;
  pro.set_output_val<vil_image_view_base_sptr>(j++, img);
  pro.set_output_val<vil_image_view_base_sptr>(j++, img_mask);
  return true;
}
