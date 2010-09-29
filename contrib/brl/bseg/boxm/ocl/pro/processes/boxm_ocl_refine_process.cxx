// This is brl/bseg/boxm/ocl/pro/processes/boxm_ocl_refine_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for refining a scene using OpenCL GPU acceleration
//
// \author Andrew Miller
// \date June 16, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <boxm/ocl/boxm_refine_scene_manager.h>

namespace boxm_ocl_refine_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 0;
}

bool boxm_ocl_refine_process_cons(bprb_func_process& pro)
{
  using namespace boxm_ocl_refine_process_globals;
  // process takes 3 inputs and has no outputs
  // input[0]: scene.xml file path
  // input[1]: prob_thresh
  // input[2]: output_directory
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "float";
  input_types_[2] = "vcl_string";

  return pro.set_input_types(input_types_);
}

bool boxm_ocl_refine_process(bprb_func_process& pro)
{
  using namespace boxm_ocl_refine_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vcl_string scene_path = pro.get_input<vcl_string>(i++);
  float prob_thresh = pro.get_input<float>(i++);
  vcl_string out_path = pro.get_input<vcl_string>(i++);

  //refine scene
  vcl_cout<<"Refining OCL scene: "<<scene_path<<" with prob "<<prob_thresh<<vcl_endl;
  boxm_ocl_scene scene = boxm_ocl_scene(scene_path);
  vcl_cout<<scene<<vcl_endl;
  boxm_refine_scene_manager* mgr = boxm_refine_scene_manager::instance();
  mgr->init_refine(&scene, prob_thresh);
  mgr->run_refine();

  //save scene
  vcl_cout<<scene<<vcl_endl
          <<"Saving scene to "<<out_path<<vcl_endl;
  scene.save_scene(out_path);
  return true;
}
