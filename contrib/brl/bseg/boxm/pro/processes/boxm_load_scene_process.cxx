// This is brl/bseg/boxm/pro/processes/boxm_load_scene_process.cxx
//:
// \file
// \brief A class for obtaining roc curve from change detection results.
//
// \author Gamze Tunali
// \date April 14, 2009

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>

namespace boxm_load_scene_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}

bool boxm_load_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_load_scene_process_globals;
  //process takes 2 inputs
  //input[0]: scene binary file
  //input[1]: APM type
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

bool boxm_load_scene_process(bprb_func_process& pro)
{
  using namespace boxm_load_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vcl_string scene_file = pro.get_input<vcl_string>(i++);
  
  boxm_scene_base_sptr scene= new boxm_scene_base();
  boxm_scene_parser parser;
  scene->load_scene(scene_file, parser);
  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
    boxm_scene<tree_type> *s = new boxm_scene<tree_type>();
    s->load_scene(parser);
   // scene.unref(scene);
    scene = s;
  } else {
    vcl_cout << "boxm_load_scene_process: undefined APM type" << vcl_endl;
    return false;
  }

  unsigned j = 0;
  // store scene smaprt pointer
  pro.set_output_val<boxm_scene_base_sptr>(j++, scene);
  return true;
}
