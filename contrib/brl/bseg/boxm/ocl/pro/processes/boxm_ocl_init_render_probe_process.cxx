// This is brl/bseg/boxm/ocl/pro/processes/boxm_ocl_init_render_probe_process.cxx
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

namespace boxm_ocl_init_render_probe_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

bool boxm_ocl_init_render_probe_process_cons(bprb_func_process& pro)
{
  using namespace boxm_ocl_init_render_probe_process_globals;
  // process takes 3 inputs and has no outputs
  // input[0]: scene.xml file path
  // input[1]: prob_thresh
  // input[2]: output_directory
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";

  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm_render_probe_manager_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;

}

bool boxm_ocl_init_render_probe_process(bprb_func_process& pro)
{
  using namespace boxm_ocl_init_render_probe_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vcl_string scene_file = pro.get_input<vcl_string>(i++);
  boxm_ocl_scene scene(scene_file);

  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);

  unsigned pi = pro.get_input<unsigned>(i++);
  unsigned pj = pro.get_input<unsigned>(i++);
  boxm_render_probe_manager* mgr = boxm_render_probe_manager::instance();

  mgr->init_ray_trace(&scene,cam,pi,pj);
  mgr->setup_online_processing();

  pro.set_output_val<boxm_render_probe_manager_sptr>(0,boxm_render_probe_manager_sptr(mgr));
  return true;
}
