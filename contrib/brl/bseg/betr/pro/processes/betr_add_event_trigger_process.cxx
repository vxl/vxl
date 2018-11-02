// This is brl/bseg/betr/pro/processes/betr_add_event_trigger_process.cxx
#include <iostream>
#include <fstream>
#include <string>
#include <bprb/bprb_func_process.h>
#include <vpgl/vpgl_lvcs.h>
//:
// \file
// \brief  A process for adding a betr_event_trigger to a site
//


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <betr/betr_event_trigger.h>
#include <vgl/vgl_point_3d.h>
#include <betr/betr_site.h>

namespace betr_add_event_trigger_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}

bool betr_add_event_trigger_process_cons(bprb_func_process& pro)
{
  using namespace betr_add_event_trigger_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "betr_site_sptr"; //site
  input_types_[1] = "betr_event_trigger_sptr"; //site
  // process has 0 outputs
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_add_event_trigger_process(bprb_func_process& pro)
{
  using namespace betr_add_event_trigger_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  betr_site_sptr site = pro.get_input<betr_site_sptr>(i++);
  betr_event_trigger_sptr event_trigger = pro.get_input<betr_event_trigger_sptr>(i);
  bool success = site->add_event_trigger(event_trigger);
  return success;
}
