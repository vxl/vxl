// This is brl/bpro/core/vpgl_pro/processes/vpgl_save_lvcs_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/vpgl_lvcs.h>

//: initialization
bool vpgl_save_lvcs_process_cons(bprb_func_process& pro)
{
  //this process takes four inputs:
  // 0: (float)  latitude
  // 1: (float)  longitude
  // 2: (float)  elevation
  // 3: (string) lvcs filename to save
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("float");
  input_types.push_back("float");
  input_types.push_back("float");
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // process has no outputs
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_save_lvcs_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != pro.input_types().size()) {
    vcl_cout << "vpgl_save_lvcs_process: The number of inputs should be " << pro.input_types().size() << vcl_endl;
    return false;
  }

  // get the inputs
  float lat = pro.get_input<float>(0);
  float lon = pro.get_input<float>(1);
  float el = pro.get_input<float>(2);
  vcl_string lvcs_filename = pro.get_input<vcl_string>(3);

  // create lvcs
  vpgl_lvcs lvcs(lat, lon, el, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  // save to file
  vcl_ofstream ofs(lvcs_filename.c_str());
  if (!ofs.good()) {
    vcl_cerr << "Error opening lvcs filename " << lvcs_filename << '\n';
    return false;
  }

  lvcs.print(ofs);

  return true;
}

