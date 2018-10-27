// This is brl/bseg/boxm2/volm/pro/process/boxm2_location_pin_point_ranking_process
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to return the ranking of a location in the pin-point list
// NOTE: the rank begins by 1st
//
// \author Yi Dong
// \date March 18, 2015
#include <vul/vul_file.h>
#include <vgl/vgl_polygon.h>
#include <bkml/bkml_parser.h>

namespace boxm2_location_pin_point_ranking_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm2_location_pin_point_ranking_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_location_pin_point_ranking_process_globals;
  // process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "double";      // location longitude
  input_types_[1] = "double";      // location latitude
  input_types_[2] = "vcl_string";  // pin-point kml file
  // process takes 1 output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "unsigned";   // location rank inside pin-point regions
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_location_pin_point_ranking_process(bprb_func_process& pro)
{
  using namespace boxm2_location_pin_point_ranking_process_globals;
  // sanity check
  if (!pro.verify_inputs())
    return false;
  // get the inputs
  unsigned in_i = 0;
  auto lon = pro.get_input<double>(in_i++);
  auto lat = pro.get_input<double>(in_i++);
  std::string pin_point_kml = pro.get_input<std::string>(in_i++);

  // parse the pin-point regions
  if (!vul_file::exists(pin_point_kml)) {
    std::cerr << pro.name() << ": can not find file: " << pin_point_kml << std::endl;
    return false;
  }
  vgl_polygon<double> pin_polys = bkml_parser::parse_polygon(pin_point_kml);
  unsigned n_region = pin_polys.num_sheets();
  bool found = false;
  unsigned rank = 0;
  for (unsigned i = 0; (i < n_region && !found); i++) {
    vgl_polygon<double> poly(pin_polys[i]);
    if (poly.contains(lon, lat)) {
      found = true;
      rank = i+1;
    }
  }
  if (!rank) {
    std::cerr << pro.name() << ": the location (" << lon << ", " << lat << ") is not in any pin-point region" << std::endl;
    return false;
  }
  // output
  pro.set_output_val<unsigned>(0, rank);
  return true;
}
