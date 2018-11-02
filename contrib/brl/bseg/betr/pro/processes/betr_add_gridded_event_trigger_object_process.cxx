// This is brl/bseg/betr/pro/processes/betr_add_gridded_event_trigger_object_process.cxx
#include <iostream>
#include <fstream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//:
// \file
// \brief  A process for adding an event_trigger object to an event_trigger
//


#include <betr/betr_event_trigger.h>
#include <vgl/vgl_point_3d.h>

namespace betr_add_gridded_event_trigger_object_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
}

bool betr_add_gridded_event_trigger_object_process_cons(bprb_func_process& pro)
{
  using namespace betr_add_gridded_event_trigger_object_process_globals;

  //process takes 7 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "betr_event_trigger_sptr"; //event_trigger
  input_types_[1] = "vcl_string";//name
  input_types_[2] = "float";// lon
  input_types_[3] = "float";// lat
  input_types_[4] = "float";// elev
  input_types_[5] = "vcl_string";// geom path for enclosing polygon
  input_types_[6] = "float"; // grid spacing
  // process has 0 outputs
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_add_gridded_event_trigger_object_process(bprb_func_process& pro)
{
  using namespace betr_add_gridded_event_trigger_object_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  betr_event_trigger_sptr event_trigger = pro.get_input<betr_event_trigger_sptr>(i++);
  std::string name = pro.get_input<std::string>(i++);
  auto lon = pro.get_input<float>(i++);
  auto lat = pro.get_input<float>(i++);
  auto elev = pro.get_input<float>(i++);
  std::string geom_path = pro.get_input<std::string>(i++);
  auto grid_spacing = pro.get_input<float>(i);
  if(!event_trigger)
    return false;
  bool good = event_trigger->add_gridded_event_poly(name, lon, lat, elev, geom_path, grid_spacing);
  return good;
}
