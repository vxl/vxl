// This is brl/bseg/boxm/opt/open_cl/pro/processes/boxm_opt_opencl_render_expected_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for rendering an expected image using OpenCL GPU acceleration
//
// \author Daniel Crispell
// \date March 15, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/opt/open_cl/boxm_opt_opencl_render_expected.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_sample_multi_bin.h>

#include <vil/vil_convert.h>

namespace boxm_opt_opencl_render_expected_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 2;
}

bool boxm_opt_opencl_render_expected_process_cons(bprb_func_process& pro)
{
  using namespace boxm_opt_opencl_render_expected_process_globals;

  //process takes 5 inputs
  //input[0]: scene binary file
  //input[1]: camera
  //input[2]: ni of the expected image
  //input[3]: nj of the expected image
  //input[4]: black background?
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  input_types_[4] = "bool";

  // process has 2 outputs:
  // output[0]: rendered image
  // output[1]: mask image
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_opt_opencl_render_expected_process(bprb_func_process& pro)
{
  using namespace boxm_opt_opencl_render_expected_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);
  bool use_black_background =  pro.get_input<bool>(i++);

  vil_image_view_base_sptr img;
  vil_image_view_base_sptr img_mask;

  // check the scene's appearance model
  switch (scene_ptr->appearence_model())
  {
    case BOXM_APM_MOG_GREY:
    {
      vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> expected(ni,nj);
      vil_image_view<float> mask(ni,nj);
      if (!scene_ptr->multi_bin())
      {
        typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > type;
        boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
        boxm_opencl_render_expected<BOXM_APM_MOG_GREY>(*scene, camera, expected, mask, use_black_background);
      }
      else
      {
        vcl_cout<<"OpenCL rendering of multi-bin scenes not implemented"<<vcl_endl;
        return false;
      }
      img_mask = new vil_image_view<float>(mask);

      vil_image_view<unsigned char> *expected_byte = new vil_image_view<unsigned char>(expected.ni(),expected.nj(),expected.nplanes());
      vil_convert_stretch_range_limited(expected,*expected_byte, 0.0f, 1.0f);
      img = expected_byte;
      break;
    }
    case BOXM_APM_SIMPLE_GREY:
    {
      vil_image_view<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> expected(ni,nj);
      vil_image_view<float> mask(ni,nj);
      if (!scene_ptr->multi_bin())
      {
        typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > type;
        boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
        boxm_opencl_render_expected<BOXM_APM_SIMPLE_GREY>(*scene, camera, expected, mask, use_black_background);
      }
      else
      {
        vcl_cout<<"OpenCL rendering of multi-bin scenes not implemented"<<vcl_endl;
        return false;
      }
      //img = new vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype>(expected);
      img_mask = new vil_image_view<float>(mask);

      vil_image_view<unsigned char> *expected_byte = new vil_image_view<unsigned char>(expected.ni(),expected.nj(),expected.nplanes());
      vil_convert_stretch_range_limited(expected,*expected_byte, 0.0f, 1.0f);
      img = expected_byte;
      break;
    }
    default:
    {
      vcl_cout << "boxm_opt_opencl_render_expected_process: unsupported APM type" << vcl_endl;
      return false;
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, img);
  pro.set_output_val<vil_image_view_base_sptr>(1, img_mask);

  return true;
}
