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
#include <boxm/ocl/boxm_ocl_bit_scene.h>
#include <boxm/ocl/boxm_update_bit_scene_manager.h>
#include <boxm/ocl/boxm_update_bit_scene_manager_sptr.h>

namespace boxm_init_update_bit_scene_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

bool boxm_init_update_bit_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_init_update_bit_scene_process_globals;
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

bool boxm_init_update_bit_scene_process(bprb_func_process& pro)
{
  using namespace boxm_init_update_bit_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_ocl_bit_scene_sptr scene = pro.get_input<boxm_ocl_bit_scene_sptr>(i++);
  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);
  float thresh= pro.get_input<float>(i++);

  boxm_update_bit_scene_manager* mgr = boxm_update_bit_scene_manager::instance();
  mgr->set_bundle_ni(8);
  mgr->set_bundle_nj(8);
  mgr->init_scene(scene.as_pointer(),ni,nj,thresh);
  if (!mgr->setup_norm_data(true, 0.5f, 0.25f))
      return -1;

  mgr->setup_online_processing();
  pro.set_output_val<boxm_update_bit_scene_manager_sptr>(0,boxm_update_bit_scene_manager_sptr(mgr));
  return true;
}
