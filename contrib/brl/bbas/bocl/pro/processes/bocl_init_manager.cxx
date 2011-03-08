// This is brl/bseg/boxm/ocl/pro/processes/boxm_init_update_bit_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for initializing a n update bit  scene  manager using OpenCL 
//
// \author Vishal Jain
// \date Sept 29, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <bocl/bocl_manager.h>


namespace bocl_init_manager_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

bool bocl_init_manager_process_cons(bprb_func_process& pro)
{
  using namespace bocl_init_manager_process_globals;
  // process takes 3 inputs and has no outputs
  // input[0]: scene.xml file path
  // input[1]: prob_thresh
  // input[2]: output_directory
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_ocl_bit_scene_sptr";
  input_types_[1] = "unsigned";
  input_types_[2] = "unsigned";
  input_types_[3] = "float";

  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm_update_bit_scene_manager_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;

}

bool bocl_init_manager_process_process(bprb_func_process& pro)
{
  using namespace bocl_init_manager_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;


  bocl_manager_child_sptr mgr = new bocl_manager_child();
  pro.set_output_val<bocl_manager_sptr>(0,bocl_manager_sptr(mgr));
  return true;
}
