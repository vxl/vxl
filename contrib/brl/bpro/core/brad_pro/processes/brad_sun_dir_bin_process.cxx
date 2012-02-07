// This is brl/bpro/core/brad_pro/processes/brad_sun_dir_bin_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
#include <brad/brad_sun_dir_index.h>
#include <vnl/vnl_double_3.h>
#include <vcl_fstream.h>
#include <brad/brad_image_metadata.h>

//: Constructor
bool brad_sun_dir_bin_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brad_image_metadata_sptr");
  input_types.push_back("vcl_string"); // sun illumination bin file path
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("int");  // the bin index
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_sun_dir_bin_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    vcl_cout << "brad_sun_dir_bin_process: The input number should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  brad_image_metadata_sptr md = pro.get_input<brad_image_metadata_sptr>(i++);
  double sun_az = md->sun_azimuth_;
  double sun_el = md->sun_elevation_;
  vcl_string bin_input_path = pro.get_input<vcl_string>(i);
  vcl_ifstream is(bin_input_path.c_str());
  if (!is.is_open()) {
    vcl_cout << "In brad_sun_dir_bin_process: can't open sun direction bin file\n";
    return false;
  }
  //read the direction bin ascii file
  vcl_string temp;
  double longitude=0, latitude=0;
  is >> temp;
  if (temp != "longitude:") {
    vcl_cout << "In brad_sun_dir_bin_process, input dir file parse error\n";
    return false;
  }
  is >> longitude;
  is >> temp;
  if (temp != "latitude:") {
    vcl_cout << "In brad_sun_dir_bin_process, input dir file parse error\n";
    return false;
  }
  is >> latitude;
  is >> temp;
  if (temp != "nbins:") {
    vcl_cout << "In brad_sun_dir_bin_process, input dir file parse error\n";
    return false;
  }
  int n_bins;
  is >> n_bins;
  double x, y, z;
  vcl_vector<vnl_double_3> bins;
  for (int i = 0; i<n_bins; ++i)
  {
    is >> x >> y >> z;
    vnl_double_3 bc(x, y, z);
    bins.push_back(bc);
  }
  brad_sun_dir_index bindx(longitude, latitude, bins);
  int bin = bindx.index(sun_az, sun_el);
  pro.set_output_val<int>(0, bin);
  is.close();
  return true;
}

