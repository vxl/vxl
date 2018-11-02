// This is brl/bseg/boxm2/view/pro/processes/boxm2_view_trajectory_size_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for determining size of boxm2_trajectory object
//
// \author Daniel Crispell
// \date Mar 5, 2012

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/view/boxm2_trajectory.h>

//brdb stuff
#include <brdb/brdb_value.h>

namespace boxm2_view_trajectory_size_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm2_view_trajectory_size_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_view_trajectory_size_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_trajectory_sptr"; //trajectory object

  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "unsigned";     //number of cameras in trajectory

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_view_trajectory_size_process(bprb_func_process& pro)
{
  using namespace boxm2_view_trajectory_size_process_globals;
  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned argIdx = 0;
  boxm2_trajectory_sptr trajectory = pro.get_input<boxm2_trajectory_sptr>(argIdx++);

  // get number of views
  unsigned int size = trajectory->size();

  // store trajectory pointer
  argIdx = 0;
  pro.set_output_val<unsigned>(argIdx, size);
  return true;
}
