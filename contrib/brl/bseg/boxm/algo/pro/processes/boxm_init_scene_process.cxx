// This is brl/bseg/boxm/algo/pro/processes/boxm_init_scene_process.cxx

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
//:
// \file
// \brief A process for initializing a new boxm_scene when there is not a saved one.
//        It receives the parameters from a parameter file in XML
//
// \author Vishal Jain
// \date Feb 8, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/boxm_scene_parser.h>
#include <boxm/algo/boxm_init_scene.h>

//: global variables
namespace boxm_init_scene_process_globals
{
  //this process takes no outputs
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 0;
}

//: sets input and output types
bool boxm_init_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_init_scene_process_globals;

  // process takes 1 input and no outputs
  //input[0]: The scene
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";

  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: creates a scene from parameters
bool boxm_init_scene_process(bprb_func_process& pro)
{
  using namespace boxm_init_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << "boxm_init_scene_process: The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);

  // check the input validity
  if (scene == 0) {
    vcl_cout << "boxm_init_scene_process: scene is null, cannot run" << vcl_endl;
    return false;
  }

  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    if (!scene->multi_bin()) {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_init_scene<BOXM_APM_MOG_GREY>(*s);
    }
  }
  else if (scene->appearence_model() == BOXM_APM_SIMPLE_GREY) {
    if (scene->multi_bin()) {
      vcl_cout << "boxm_init_scene_process: multibin case is not implemented for BOXM_APM_SIMPLE_GREY yet" << vcl_endl;
      return false;
    }
    else {
      typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_init_scene<BOXM_APM_SIMPLE_GREY>(*s);
    }
  }
  else if (scene->appearence_model() == BOXM_APM_MOB_GREY) {
    if (scene->multi_bin()) {
      vcl_cout << "boxm_init_scene_process: multibin case is not implemented for BOXM_APM_SIMPLE_GREY yet" << vcl_endl;
      return false;
    }
    else {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_init_scene<BOXM_APM_MOB_GREY >(*s);
    }
  }
  else {
    vcl_cout << "boxm_init_scene_process: undefined APM type" << vcl_endl;
    return false;
  }

  return true;
}
