// This is brl/bpro/core/brad_pro/processes/brad_sun_dir_bin_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <brad/brad_sun_dir_index.h>
#include <vnl/vnl_double_3.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brad/brad_image_metadata.h>

//: Constructor
bool brad_sun_dir_bin_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brad_image_metadata_sptr");
  input_types.emplace_back("vcl_string"); // sun illumination bin file path
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("int");  // the bin index
  output_types.emplace_back("float"); // float
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_sun_dir_bin_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "brad_sun_dir_bin_process: The input number should be 2" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  brad_image_metadata_sptr md = pro.get_input<brad_image_metadata_sptr>(i++);
  double sun_az = md->sun_azimuth_;
  double sun_el = md->sun_elevation_;
  std::string bin_input_path = pro.get_input<std::string>(i);
  std::ifstream is(bin_input_path.c_str());
  if (!is.is_open()) {
    std::cout << "In brad_sun_dir_bin_process: can't open sun direction bin file\n";
    return false;
  }
  //read the direction bin ascii file
  std::string temp;
  double longitude=0, latitude=0;
  is >> temp;
  if (temp != "longitude:") {
    std::cout << "In brad_sun_dir_bin_process, input dir file parse error\n";
    return false;
  }
  is >> longitude;
  is >> temp;
  if (temp != "latitude:") {
    std::cout << "In brad_sun_dir_bin_process, input dir file parse error\n";
    return false;
  }
  is >> latitude;
  is >> temp;
  if (temp != "nbins:") {
    std::cout << "In brad_sun_dir_bin_process, input dir file parse error\n";
    return false;
  }
  int n_bins;
  is >> n_bins;
  double x, y, z;
  std::vector<vnl_double_3> bins;
  for (int i = 0; i<n_bins; ++i)
  {
    is >> x >> y >> z;
    vnl_double_3 bc(x, y, z);
    bins.push_back(bc);
  }
  brad_sun_dir_index bindx(longitude, latitude, bins);
  double angle;
  int bin = bindx.index(sun_az, sun_el, angle);
  pro.set_output_val<int>(0, bin);
  pro.set_output_val<float>(1, angle);
  is.close();
  return true;
}
