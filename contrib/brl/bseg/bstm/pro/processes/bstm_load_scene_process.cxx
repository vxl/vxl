// This is brl/bseg/bstm/pro/processes/bstm_load_scene_process.cxx
//:
// \file
// \brief  A process for loading the scene.
//
// \author Ali Osman Ulusoy
// \date Nov 27, 2012

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <bstm/bstm_scene.h>

namespace bstm_load_scene_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}
bool bstm_load_scene_process_cons(bprb_func_process& pro)
{
  using namespace bstm_load_scene_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "bstm_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_load_scene_process(bprb_func_process& pro)
{
  using namespace bstm_load_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  vcl_string scene_file = pro.get_input<vcl_string>(i++);
  bstm_scene_sptr scene= new bstm_scene(scene_file);

  i=0;
  // store scene smaprt pointer
  pro.set_output_val<bstm_scene_sptr>(i++, scene);
  return true;
}
