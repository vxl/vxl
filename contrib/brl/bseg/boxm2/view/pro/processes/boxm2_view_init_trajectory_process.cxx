// This is brl/bseg/boxm2/view/pro/processes/boxm2_view_init_trajectory_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for initializing a boxm2_trajectory 
//
// \author Andrew Miller
// \date Mar 10, 2011

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/view/boxm2_trajectory.h>

//brdb stuff
#include <brdb/brdb_value.h>

namespace boxm2_view_init_trajectory_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 1;
}

bool boxm2_view_init_trajectory_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_view_init_trajectory_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr"; //Scene 
  input_types_[1] = "double"; //incline 0
  input_types_[2] = "double"; //incline 1
  input_types_[3] = "double"; //radius
  input_types_[4] = "unsigned"; // ni image width
  input_types_[5] = "unsigned"; // nj image height

  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_trajectory_sptr";     //an initialized trajectory object

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_view_init_trajectory_process(bprb_func_process& pro)
{
  using namespace boxm2_view_init_trajectory_process_globals;
  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned argIdx = 0;
  boxm2_scene_sptr scene    = pro.get_input<boxm2_scene_sptr>(argIdx++);
  double           incline0 = pro.get_input<double>(argIdx++);
  double           incline1 = pro.get_input<double>(argIdx++);
  double           radius   = pro.get_input<double>(argIdx++);
  unsigned         ni       = pro.get_input<unsigned>(argIdx++);
  unsigned         nj       = pro.get_input<unsigned>(argIdx++);

  //new trjaectory
  boxm2_trajectory_sptr traj = new boxm2_trajectory(incline0, incline1, radius, scene->bounding_box(), ni, nj); 

  // store trajectory pointer
  argIdx = 0;
  pro.set_output_val<boxm2_trajectory_sptr>(argIdx, traj);
  return true;
}
