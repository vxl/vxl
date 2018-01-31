// This is brl/bseg/betr/pro/processes/betr_execute_event_trigger_multi_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <vcl_string.h>
//:
// \file
// \brief  A process for executing an event_trigger (process change)
//


#include <vcl_compiler.h>
#include <betr/betr_event_trigger.h>
#include <vgl/vgl_point_3d.h>

namespace betr_execute_event_trigger_multi_process_globals
{
  const unsigned n_inputs_  = 3;
  const unsigned n_outputs_ = 2;
}

bool betr_execute_event_trigger_multi_process_cons(bprb_func_process& pro)
{
  using namespace betr_execute_event_trigger_multi_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0]  = "betr_event_trigger_sptr"; //event_trigger
  input_types_[1]  = "vcl_string"; //algorithm name
  input_types_[2]  = "vcl_string"; //algorithm name
  // process has 1 output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_double_sptr"; // change probability
  output_types_[1] = "bbas_1d_array_string_sptr"; // event region names
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_execute_event_trigger_multi_process(bprb_func_process& pro)
{
  using namespace betr_execute_event_trigger_multi_process_globals;
  
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  betr_event_trigger_sptr event_trigger = pro.get_input<betr_event_trigger_sptr>(i++);
  std::string algorithm_name = pro.get_input<std::string>(i++);
  std::string algorithm_params = pro.get_input<std::string>(i);
  if(!event_trigger)
    return false;
  std::cout << "executing " << algorithm_name << " with params " << algorithm_params << std::endl;
  const std::map<std::string, betr_geo_object_3d_sptr>& evt_objs = event_trigger->evt_objects();
  unsigned n = evt_objs.size();
  bbas_1d_array_double_sptr change_probs = new bbas_1d_array_double(n);
  bbas_1d_array_string_sptr evt_names = new bbas_1d_array_string(n);
  unsigned j = 0;
  for(std::map<std::string, betr_geo_object_3d_sptr>::const_iterator oit = evt_objs.begin();
      oit != evt_objs.end(); ++oit, ++j)
    evt_names->data_array[j]=oit->first;
  std::vector<double> prob_change;
  bool good = event_trigger->process(algorithm_name, prob_change, algorithm_params);
  if(!good)
    return good;
  j = 0;
  for(std::vector<double>::iterator pit = prob_change.begin();
      pit != prob_change.end(); ++pit, ++j)
    change_probs->data_array[j]=*pit;
  pro.set_output_val<bbas_1d_array_double_sptr>(0, change_probs);
  pro.set_output_val<bbas_1d_array_string_sptr>(1, evt_names);
  return good;
}
