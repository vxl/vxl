// This is brl/bseg/boxm/pro/processes/boxm_update_process.cxx
//:
// \file
// \brief Process to update the scene from an image and camera pair
//
// \author Gamze Tunali
// \date 04/21/2009
// \verbatim
//  Modifications
//   
// \endverbatim

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_update.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

namespace boxm_update_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 1;
}

bool boxm_update_process_cons(bprb_func_process& pro)
{
  using namespace boxm_update_process_globals;
  //process takes 4inputs
  //input[0]: The observation image
  //input[1]: The camera of the observation
  //input[2]: The scene
  //input[3]: The bin index to be updated
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "boxm_scene_base_sptr";
  input_types_[3] = "unsigned";
  if(!pro.set_input_types(input_types_))
    return false;

  //no output 
  return true;
}

bool boxm_update_process(bprb_func_process& pro)
{
  using namespace boxm_update_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << "boxm_update_process: The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  vil_image_view_base_sptr input_image = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  unsigned bin_index = pro.get_input<unsigned>(i++);

  // check the input validity
  if ((input_image == 0) || (camera == 0) || (scene == 0)) {
     vcl_cout << "boxm_update_process: null input value, cannot run" << vcl_endl;
     return false;
  }

  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
    boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
    vil_image_view<float> image = *vil_convert_cast(float(), input_image);
    boxm_update<short, BOXM_APM_MOG_GREY>(*s, image, camera, true);
  } else {
    vcl_cout << "boxm_update_process: undefined APM type" << vcl_endl;
    return false;
  }

  return true;
}
