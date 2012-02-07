// This is brl/bpro/core/brad_pro/processes/brad_display_sun_index_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_sun_dir_index.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_display_sun_index_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); // vrml output file name
  input_types.push_back("float");      // longitude (degrees)
  input_types.push_back("float");      // latitude (degrees)
  input_types.push_back("int");        // year, e.g. 2002 (start of interval)
  input_types.push_back("int");        // observation hour e.g. 10 or 22 (zulu)
  input_types.push_back("int");        // observation minute
  input_types.push_back("int");        // bin radius nbins = 2 x radius + 1
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types; // no outputs
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_display_sun_index_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 7) {
    vcl_cout << "brad_display_sun_index_process: The input number should be 7" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //output file name
  vcl_string out_file = pro.get_input<vcl_string>(i++);
  vcl_ofstream os(out_file.c_str());
  if (!os.is_open()) {
    vcl_cout << "in display_sun_index_process, couldn't open output file stream\n";
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

