// This is brl/bseg/boxm/opt/pro/processes/boxm_generate_edge_samples_process.cxx
//:
// \file
// \brief Process to update the scene from an image and camera pair
//
// \author Gamze Tunali
// \date Sep 15, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/opt/boxm_generate_edge_sample_functor.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <boxm/boxm_sample.h>

namespace boxm_generate_edge_samples_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

bool boxm_generate_edge_samples_process_cons(bprb_func_process& pro)
{
  using namespace boxm_generate_edge_samples_process_globals;
  //process takes 4 inputs
  //input[0]: The cropped edge image
  //input[1]: The camera of the observation
  //input[2]: The scene
  //input[3]: image name for saving scene
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "boxm_scene_base_sptr";
  input_types_[3] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;

  //no output
  return true;
}

bool boxm_generate_edge_samples_process(bprb_func_process& pro)
{
  using namespace boxm_generate_edge_samples_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << "boxm_generate_edge_samples_process: The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  vil_image_view_base_sptr edge_image = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  vcl_string img_name =  pro.get_input<vcl_string>(i++); 

  // check the input validity
  if ((edge_image == 0) || (camera == 0) || (scene == 0)) {
     vcl_cout << "boxm_generate_edge_samples_process: null input value, cannot run" << vcl_endl;
     return false;
  }

  // edge image is always float??
  if (scene->appearence_model() == BOXM_EDGE_FLOAT) {
    vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(edge_image.ptr());
    vil_image_view<float> img(img_byte->ni(), img_byte->nj(), 1);
    vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
    if (!scene->multi_bin())
    {
      typedef boct_tree<short, boxm_edge_sample<float> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_generate_edge_sample_rt<short,boxm_edge_sample<float> > (*s, camera, img, img_name);
    }
    else
    {
      vcl_cout<<"boxm_generate_edge_samples_process: Multibin -- Not yet implemented"<<vcl_endl;
    }
  }
  else {
    vcl_cout << "boxm_generate_edge_samples_process: undefined APM type" << vcl_endl;
    return false;
  }

  return true;
}
