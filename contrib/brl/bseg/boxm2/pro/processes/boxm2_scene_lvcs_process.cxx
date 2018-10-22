// This is brl/bseg/boxm2/pro/processes/boxm2_scene_lvcs_process.cxx
//:
// \file
// \brief  A process for accessing scene LVCS
//
// \author Ozge C. Ozcanli
// \date May 11, 2011

#include <bprb/bprb_func_process.h>

#include <boxm2/boxm2_scene.h>
#include <vpgl/vpgl_lvcs_sptr.h>

namespace boxm2_scene_lvcs_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}
bool boxm2_scene_lvcs_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_lvcs_process_globals;
  //process takes 1 input, the scene
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";

  // process has 3 outputs:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_lvcs_sptr";
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_scene_lvcs_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_lvcs_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  boxm2_scene_sptr   scene = pro.get_input<boxm2_scene_sptr>(0);
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());

  // store outputs
  int i=0;
  pro.set_output_val<vpgl_lvcs_sptr>(i++, lvcs);
  return true;
}
