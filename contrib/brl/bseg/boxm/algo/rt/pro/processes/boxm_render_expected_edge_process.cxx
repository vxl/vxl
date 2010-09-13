// This is brl/bseg/boxm/algo/rt/pro/processes/boxm_render_expected_edge_process.cxx
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

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/rt/boxm_render_expected_edge_image_functor.h>
#include <boxm/algo/rt/boxm_render_expected_edge_tangent_image_functor.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/sample/boxm_inf_line_sample.h>
#include <vil/vil_image_view.h>
#include <vcl_fstream.h>

namespace boxm_render_expected_edge_process_globals
{
  const unsigned n_inputs_ = 7;
  const unsigned n_outputs_ = 2;
}

bool boxm_render_expected_edge_process_cons(bprb_func_process& pro)
{
  using namespace boxm_render_expected_edge_process_globals;

  //process takes 7 inputs
  //input[0]: scene binary file
  //input[1]: camera
  //input[2]: ni of the expected image
  //input[3]: nj of the expected image
  //input[4]: n_normal
  //input[5]: num samples used for update
  //input[6]: Threshold for edges
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  input_types_[4] = "float";
  input_types_[5] = "unsigned";
  input_types_[6] = "float";

  // process has 2 outputs:
  // output[0]: rendered image
  // output[1]: mask
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_render_expected_edge_process(bprb_func_process& pro)
{
  using namespace boxm_render_expected_edge_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cerr << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);
  float n_normal = pro.get_input<float>(i++);
  // number of updates
  unsigned num_updates = pro.get_input<unsigned>(i++);
  //float threshold = pro.get_input<float>(i++); // FIXME: unused; see line 138
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
      if (!scene) {
        vcl_cerr << "boxm_render_expected_edge_process: the scene is not of expected type\n";
        return false;
      }
      boxm_render_edge_image_rt<short, boxm_edge_sample<float> >(*scene, camera, expected, mask,n_normal,num_updates);
    }
    else
    {
      vcl_cerr << "Ray tracing version not yet implemented\n";
      return false;
    }
    img_mask = new vil_image_view<float>(mask);

    vil_image_view<vxl_byte> *expected_byte = new vil_image_view<vxl_byte>(ni,nj,expected.nplanes());
    for (unsigned i=0; i<ni; i++) {
      for (unsigned j=0; j<nj; j++) {
        (*expected_byte)(i,j) = static_cast<unsigned char>(255.0*(expected(i,j)));
      }
    }
    img = expected_byte;
        img_mask = new vil_image_view<float>(mask);

    vil_image_view<float> *ex = new vil_image_view<float>(ni,nj,expected.nplanes());
    for (unsigned i=0; i<ni; i++) {
      for (unsigned j=0; j<nj; j++) {
        (*ex)(i,j) = expected(i,j);//static_cast<unsigned char>(255.0*(expected(i,j)));
      }
    }
    img = ex;
      unsigned j = 0;
    pro.set_output_val<vil_image_view_base_sptr>(j++, img);
    pro.set_output_val<vil_image_view_base_sptr>(j++, img_mask);
    return true;
  }

  if (scene_ptr->appearence_model() == BOXM_EDGE_LINE) {
    vil_image_view<float > expected(ni,nj,1);
    vil_image_view<float> mask(ni,nj);
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short, boxm_inf_line_sample<float> > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
      if (!scene) {
        vcl_cerr << "boxm_render_expected_edge_process: the scene is not of expected type\n";
        return false;
      }
      boxm_render_edge_tangent_image_rt<short, boxm_inf_line_sample<float> >(*scene, camera, expected);//, n_normal,num_updates, threshold);

      vil_image_view<float > *edge_image=new  vil_image_view<float >(ni,nj,1);
      *edge_image=expected;

      unsigned j = 0;
      pro.set_output_val<vil_image_view_base_sptr>(j++,edge_image);
      pro.set_output_val<vil_image_view_base_sptr>(j++,edge_image);
      return true;
    }
    else
    {
      vcl_cerr << "Ray tracing version not yet implemented\n";
      return false;
    }
  }
  else {
    vcl_cerr << "boxm_render_expected_edge_process: undefined APM type\n";
    return false;
  }
}
