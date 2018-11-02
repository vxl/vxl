// This is brl/bseg/boxm/pro/processes/boxm_load_scene_process.cxx
//:
// \file
// \brief A class for obtaining roc curve from change detection results.
//
// \author Gamze Tunali
// \date April 14, 2009

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>

namespace boxm_load_scene_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm_load_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_load_scene_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_load_scene_process(bprb_func_process& pro)
{
  using namespace boxm_load_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  std::string scene_file = pro.get_input<std::string>(i++);

  boxm_scene_base_sptr scene= new boxm_scene_base();
  boxm_scene_parser parser;
  scene->load_scene(scene_file, parser);
  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
    auto *s = new boxm_scene<tree_type>();
    s->load_scene(parser);
   // scene.unref(scene);
    scene = s;
  } else {
    std::cout << "boxm_load_scene_process: undefined APM type" << std::endl;
    return false;
  }

  unsigned j = 0;
  // store scene smaprt pointer
  pro.set_output_val<boxm_scene_base_sptr>(j++, scene);
  return true;
}
