// This is brl/bseg/bstm/pro/processes/bstm_load_scene_process.cxx
//:
// \file
// \brief  A process for loading the scene.
//
// \author Ali Osman Ulusoy
// \date Nov 27, 2012

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/bstm_scene.h>

namespace bstm_load_scene_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}
bool bstm_load_scene_process_cons(bprb_func_process& pro)
{
  using namespace bstm_load_scene_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "bstm_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_load_scene_process(bprb_func_process& pro)
{
  using namespace bstm_load_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  std::string scene_file = pro.get_input<std::string>(i++);
  bstm_scene_sptr scene= new bstm_scene(scene_file);

  i=0;
  // store scene smaprt pointer
  pro.set_output_val<bstm_scene_sptr>(i++, scene);
  return true;
}
