// This is brl/bseg/boxm/pro/processes/boxm_refine_scene_process.cxx
//:
// \file
// \brief Process to refine the block (octrees) of the scene 
//        If there are some cells with big values they are spitted into
//        new child cells, the data of the cell is coppied to the 
//        children's data. The purspose of this process is to elaborate
//        the octree at the areas where more details exist.
//
// \author Gamze Tunali
// \date 04/24/2009
// \verbatim
//  Modifications
//   
// \endverbatim

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_refine.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

namespace boxm_refine_scene_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}

bool boxm_refine_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_refine_scene_process_globals;
  //process takes 2 inputs
  //input[0]: The scene
  //input[1]: The threshold for splitting 
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "float";
  if(!pro.set_input_types(input_types_))
    return false;

  //no output 
  return true;
}

bool boxm_refine_scene_process(bprb_func_process& pro)
{
  using namespace boxm_refine_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << "boxm_refine_scene_process: The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  float thresh = pro.get_input<float>(i++);

  // check the input validity
  if (scene == 0) {
     vcl_cout << "boxm_refine_scene_process: scene is null, cannot run" << vcl_endl;
     return false;
  }

  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
    boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
    boxm_refine<short, BOXM_APM_MOG_GREY>(*s, thresh, false);
  } else {
    vcl_cout << "boxm_refine_scene_process: undefined APM type" << vcl_endl;
    return false;
  }

  return true;
}

