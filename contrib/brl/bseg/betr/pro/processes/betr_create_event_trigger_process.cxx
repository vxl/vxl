// This is brl/bseg/betr/pro/processes/betr_create_event_trigger_process.cxx
#include <iostream>
#include <fstream>
#include <string>
#include <bprb/bprb_func_process.h>
#include <vpgl/vpgl_lvcs.h>
//:
// \file
// \brief  A process for creating a betr_event_trigger
//


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <betr/betr_event_trigger.h>
#include <vgl/vgl_point_3d.h>

namespace betr_create_event_trigger_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}

bool betr_create_event_trigger_process_cons(bprb_func_process& pro)
{
  using namespace betr_create_event_trigger_process_globals;

  //process takes 5 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "float";
  input_types_[1] = "float";
  input_types_[2] = "float";
  input_types_[3] = "vcl_string";
  input_types_[4] = "bool";
  // process has 1 output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "betr_event_trigger_sptr";
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_create_event_trigger_process(bprb_func_process& pro)
{
  using namespace betr_create_event_trigger_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  auto lon = pro.get_input<float>(i++);
  auto lat = pro.get_input<float>(i++);
  auto elev = pro.get_input<float>(i++);
  std::string name = pro.get_input<std::string>(i++);
  bool verbose = pro.get_input<bool>(i);

  vpgl_lvcs lvcs(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG);
  betr_event_trigger_sptr event_trigger = new betr_event_trigger(name, lvcs);
  event_trigger->set_verbose(verbose);
  pro.set_output_val<betr_event_trigger_sptr>(0, event_trigger);
  return true;
}
