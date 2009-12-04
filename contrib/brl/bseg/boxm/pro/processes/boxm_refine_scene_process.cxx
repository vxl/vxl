// This is brl/bseg/boxm/pro/processes/boxm_refine_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Process to refine the block (octrees) of the scene
//        If there are some cells with big values they are spitted into
//        new child cells, the data of the cell is coppied to the
//        children's data. The purspose of this process is to elaborate
//        the octree at the areas where more details exist.
//
// \author Gamze Tunali
// \date April 24, 2009

#include <vcl_fstream.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_refine.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_sample_multi_bin.h>

namespace boxm_refine_scene_process_globals
{
  const unsigned n_inputs_ = 3;
}

bool boxm_refine_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_refine_scene_process_globals;
  //process takes 2 inputs
  //input[0]: The scene
  //input[1]: The threshold for splitting
  //input[2]: bool for resetting the appearence model
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "float";
  input_types_[2] = "bool";
  if (!pro.set_input_types(input_types_))
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
  bool reset = pro.get_input<bool>(i++);

  // check the input validity
  if (scene == 0) {
     vcl_cout << "boxm_refine_scene_process: scene is null, cannot run" << vcl_endl;
     return false;
  }

  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    if (scene->multi_bin()) {
      typedef boct_tree<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_refine_scene<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(*s, thresh, reset);
    }
    else {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_refine_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, thresh, reset);
    }
  } else if (scene->appearence_model() == BOXM_APM_SIMPLE_GREY) {
    if (scene->multi_bin()) {
      vcl_cout << "boxm_refine_scene_process: multibin case is not implemented for BOXM_APM_SIMPLE_GREY yet" << vcl_endl;
      return false;
    }
    else {
      typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_refine_scene<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >(*s, thresh, reset);
    }
  }else if (scene->appearence_model() == BOXM_APM_MOB_GREY) {
    if (scene->multi_bin()) {
      vcl_cout << "boxm_refine_scene_process: multibin case is not implemented for BOXM_APM_SIMPLE_GREY yet" << vcl_endl;
      return false;
    }
    else {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_refine_scene<short, boxm_sample<BOXM_APM_MOB_GREY> >(*s, thresh, reset);
    }
  }
  else {
    vcl_cout << "boxm_refine_scene_process: undefined APM type" << vcl_endl;
    return false;
  }

  return true;
}

