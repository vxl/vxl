// This is brl/bpro/core/vpgl_pro/processes/vpgl_compute_utm_zone_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to compute UTM zone id given wgs84 lat, lon value
//
// \author Yi Dong
// \date April 14, 2014

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_utm.h>

// global variables and functions
namespace vpgl_compute_utm_zone_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 4;
}

//: initialization
bool vpgl_compute_utm_zone_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_compute_utm_zone_process_globals;
  // process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "double";         // lon
  input_types_[1] = "double";         // lat

  // process takes 4 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double";      // utm coordiantes east
  output_types_[1] = "double";      // utm coordinates north
  output_types_[2] = "int";         // utm zone id
  output_types_[3] = "int";         // utm northern index

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: execute the process
bool vpgl_compute_utm_zone_process(bprb_func_process& pro)
{
  using namespace vpgl_compute_utm_zone_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cout << pro.name() << ": Input is not valid" << std::endl;
    return false;
  }
  // get input
  unsigned i = 0;
  auto lon = pro.get_input<double>(i++);
  auto lat = pro.get_input<double>(i++);

  vpgl_utm utm;
  double x, y;
  int utm_zone, northing;
  utm.transform(lat, lon, x, y, utm_zone);
  if (lat < 0)  northing = 0;
  else          northing = 1;

  // output
  i = 0;
  pro.set_output_val<double>(i++, x);
  pro.set_output_val<double>(i++, y);
  pro.set_output_val<int>(i++, utm_zone);
  pro.set_output_val<int>(i++, northing);

  return true;
}
