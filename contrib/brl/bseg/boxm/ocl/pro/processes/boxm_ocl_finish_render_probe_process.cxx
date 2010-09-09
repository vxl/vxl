// This is brl/bseg/boxm/ocl/pro/processes/boxm_ocl_finish_render_probe_process.cxx
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
#include <boxm/ocl/boxm_render_probe_manager.h>
#include <boxm/ocl/boxm_render_probe_manager_sptr.h>

namespace boxm_ocl_finish_render_probe_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 0;
}

bool boxm_ocl_finish_render_probe_process_cons(bprb_func_process& pro)
{
  using namespace boxm_ocl_finish_render_probe_process_globals;
  // process takes 3 inputs and has no outputs
  // input[0]: scene.xml file path
  // input[1]: prob_thresh
  // input[2]: output_directory
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_render_probe_manager_sptr";

  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  if (!pro.set_output_types(output_types_))
    return false;

  return true;

}

bool boxm_ocl_finish_render_probe_process(bprb_func_process& pro)
{
  using namespace boxm_ocl_finish_render_probe_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_render_probe_manager_sptr scene_mgr = pro.get_input<boxm_render_probe_manager_sptr>(i++);
  scene_mgr->finish_online_processing();
  return true;
}
