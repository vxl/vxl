// This is brl/bpro/core/brad_pro/processes/brad_display_sun_index_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <brad/brad_sun_dir_index.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool brad_display_sun_index_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); // vrml output file name
  input_types.emplace_back("float");      // longitude (degrees)
  input_types.emplace_back("float");      // latitude (degrees)
  input_types.emplace_back("int");        // year, e.g. 2002 (start of interval)
  input_types.emplace_back("int");        // observation hour e.g. 10 or 22 (zulu)
  input_types.emplace_back("int");        // observation minute
  input_types.emplace_back("int");        // bin radius nbins = 2 x radius + 1
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types; // no outputs
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_display_sun_index_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 7) {
    std::cout << "brad_display_sun_index_process: The input number should be 7" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //output file name
  std::string out_file = pro.get_input<std::string>(i++);
  std::ofstream os(out_file.c_str());
  if (!os.is_open()) {
    std::cout << "in display_sun_index_process, couldn't open output file stream\n";
    return false;
  }
  double longitude = pro.get_input<float>(i++);
  double latitude = pro.get_input<float>(i++);
  int year = pro.get_input<int>(i++);
  int hour = pro.get_input<int>(i++);
  int minute = pro.get_input<int>(i++);
  int radius = pro.get_input<int>(i);
  int observation_time_range_minutes = 20;
  int sampling_interval_years = 5;
  brad_sun_dir_index bindx(longitude, latitude, year, hour, minute,
                           observation_time_range_minutes,
                           sampling_interval_years, radius);
  bindx.print_to_vrml(os);
  os.close();
  return true;
}
