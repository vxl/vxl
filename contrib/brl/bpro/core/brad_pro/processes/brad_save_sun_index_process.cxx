// This is brl/bpro/core/brad_pro/processes/brad_save_sun_index_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_sun_dir_index.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_save_sun_index_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); // sun direction bins output path
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
bool brad_save_sun_index_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 7) {
    vcl_cout << "brad_save_sun_index_process: The input number should be 7" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //output file name
  vcl_string out_file = pro.get_input<vcl_string>(i++);
  vcl_ofstream os(out_file.c_str());
  if (!os.is_open()) {
    vcl_cout << "in save_sun_index_process, couldn't open output file stream\n";
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

  os << "longitude: " << longitude << " latitude: " << latitude << '\n';
  int nbins = 2*radius + 1;
  os << "nbins: " << nbins << '\n';
  for (int i = 0; i<nbins; ++i) {
    vnl_double_3 bin_center = bindx.cone_axis(i);
    os << bin_center[0] <<' '<< bin_center[1] <<' '<< bin_center[2] <<'\n';
  }
  os.close();
  return true;
}

