// This is brl/bseg/boxm/opt/pro/processes/boxm_render_expected_edge_process.cxx
#include <bprb/bprb_func_process.h>
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

#include <vcl_fstream.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_render_image.h>
#include <boxm/opt/boxm_render_expected_edge_image_functor.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_sample_multi_bin.h>

#include <vil/vil_convert.h>
#include <vil/vil_save.h>

namespace boxm_render_expected_edge_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 2;
}

bool boxm_render_expected_edge_process_cons(bprb_func_process& pro)
{
  using namespace boxm_render_expected_edge_process_globals;
  //process takes 4 inputs
  //input[0]: scene binary file
  //input[1]: camera
  //input[2]: ni of the expected image
  //input[3]: nj of the expected image
  //input[4]: n_normal
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  input_types_[4] = "float";
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

bool boxm_render_expected_edge_process(bprb_func_process& pro)
{
  using namespace boxm_render_expected_edge_process_globals;

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
  float n_normal = pro.get_input<float>(i++);

  vil_image_view_base_sptr img;
  vil_image_view_base_sptr img_mask;

  // check the scene's app model
  if (scene_ptr->appearence_model() == BOXM_EDGE_FLOAT) {
    vil_image_view<boxm_apm_traits<BOXM_EDGE_FLOAT>::obs_datatype> expected(ni,nj);
    vil_image_view<float> mask(ni,nj);
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short, boxm_edge_sample<float> > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
      boxm_render_edge_image_rt<short, boxm_edge_sample<float> >(*scene, camera, expected, mask,n_normal,-1);
    }
    else
    {
       vcl_cout<<"Ray tracing version not yet implemented"<<vcl_endl;
       return false;
    }
    img_mask = new vil_image_view<float>(mask);
    vil_save(expected, "ef.tif");
    vil_image_view<unsigned char> *expected_byte = new vil_image_view<unsigned char>(ni,nj,expected.nplanes());
    vil_convert_stretch_range(expected,*expected_byte);
    /*for (unsigned i=0; i<ni; i++) {
      for (unsigned j=0; j<nj; j++) {
       (*expected_byte)(i,j) = static_cast<unsigned char>(255.0*(expected(i,j)));
      }
    }
    vil_convert_stretch_range_limited(*expected_byte,*expected_byte, (unsigned char)0, (unsigned char)255);*/
    img = expected_byte;
  } else {
    vcl_cout << "boxm_render_expected_process: undefined APM type" << vcl_endl;
    return false;
  }

  unsigned j = 0;
  pro.set_output_val<vil_image_view_base_sptr>(j++, img);
  pro.set_output_val<vil_image_view_base_sptr>(j++, img_mask);
  return true;
}
