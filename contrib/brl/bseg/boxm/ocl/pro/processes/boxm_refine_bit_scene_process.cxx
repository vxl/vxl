// This is brl/bseg/boxm/ocl/pro/processes/boxm_refine_bit_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for initializing a rendering scene using OpenCL GPU acceleration
//
// \author Vishal Jain
// \date Sept 7, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <boxm/ocl/boxm_update_bit_scene_manager.h>
#include <boxm/ocl/boxm_update_bit_scene_manager_sptr.h>
#include <boxm/basic/boxm_util_data_types.h>

namespace boxm_refine_bit_scene_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}

bool boxm_refine_bit_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_refine_bit_scene_process_globals;
  // process takes 2 inputs and has no outputs
  // input[0]: update_scene_manager
  // input[1]: ???
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_update_bit_scene_manager_sptr";
  input_types_[1] = "float";

  return pro.set_input_types(input_types_);
}

bool boxm_refine_bit_scene_process(bprb_func_process& pro)
{
  using namespace boxm_refine_bit_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_update_bit_scene_manager_sptr mgr     = pro.get_input<boxm_update_bit_scene_manager_sptr>(i++);
  float                                t     = pro.get_input<float>(i++); // TODO: unused!
  mgr->refine(); // TODO: unused!
  return true;
}
