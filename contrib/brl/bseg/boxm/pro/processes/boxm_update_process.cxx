// This is brl/bseg/boxm/pro/processes/boxm_update_process.cxx
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

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_update_triangle.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <boxm/boxm_sample_multi_bin.h>

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
  input_types_[4] = "bool";
  if (!pro.set_input_types(input_types_))
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
  unsigned bin_index =  pro.get_input<unsigned>(i++);
  bool use_black_background =  pro.get_input<bool>(i++);

  // check the input validity
  if ((input_image == 0) || (camera == 0) || (scene == 0)) {
     vcl_cout << "boxm_update_process: null input value, cannot run" << vcl_endl;
     return false;
  }

  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    vil_image_view<vxl_byte> *img_byte
        = dynamic_cast<vil_image_view<vxl_byte>*>(input_image.ptr());
    vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> img(img_byte->ni(), img_byte->nj(), 1);
    vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
    if (!scene->multi_bin())
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      //boxm_update<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, img, camera, false);
      boxm_update_triangle<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, img, camera, use_black_background);
    }
    else
    {
      vcl_cout<<"Multi Bin Update"<<vcl_endl;
      typedef boct_tree<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_update_triangle<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(*s, img, camera, bin_index,use_black_background);
    }
    //vil_image_view<float> image = *vil_convert_cast(float(), input_image);
  }
  else {
    vcl_cout << "boxm_update_process: undefined APM type" << vcl_endl;
    return false;
  }

  return true;
}
