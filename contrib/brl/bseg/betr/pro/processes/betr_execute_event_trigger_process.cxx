// This is brl/bseg/betr/pro/processes/betr_execute_event_trigger_process.cxx
//:
// \file
// \brief  A process for executing an event_trigger (process change)
//

#include <iostream>
#include <fstream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <betr/betr_event_trigger.h>
#include <vgl/vgl_point_3d.h>

namespace betr_execute_event_trigger_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}

bool betr_execute_event_trigger_process_cons(bprb_func_process& pro)
{
  using namespace betr_execute_event_trigger_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "betr_event_trigger_sptr"; //event_trigger
  input_types_[1] = "vcl_string"; //algorithm name
  input_types_[2] = "vcl_string"; //algorithm parameters
  // process has 1 output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "float"; // change probability
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_execute_event_trigger_process(bprb_func_process& pro)
{
  using namespace betr_execute_event_trigger_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  betr_event_trigger_sptr event_trigger = pro.get_input<betr_event_trigger_sptr>(i++);
  std::string algorithm_name = pro.get_input<std::string>(i++);
  std::string algorithm_params = pro.get_input<std::string>(i);
  if(!event_trigger){
    std::cout << "NULL event trigger " << std::endl;
    return false;
  }
  std::cout << "executing " << algorithm_name << " with params " << algorithm_params << std::endl;
  double prob_change = 0.0;
  bool good = event_trigger->process(algorithm_name, prob_change, algorithm_params);
  if(!good){
    std::cout << "event_trigger::process failed" << std::endl;
    return false;
  }
  pro.set_output_val<float>(0, prob_change);
  return good;
}
