// This is brl/bpro/core/vpgl_pro/processes/vpgl_create_lvcs_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>

//: initialization
bool vpgl_create_lvcs_process_cons(bprb_func_process& pro)
{
  //this process takes four inputs:
  // 0: (double)  latitude
  // 1: (double)  longitude
  // 2: (double)  elevation
  // 3: (string)  lvcs csname (wgs84, nad27n, wgs72, utm)
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // process has no outputs
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_lvcs_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_create_lvcs_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != pro.input_types().size()) {
    std::cout << "vpgl_create_lvcs_process: The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }

  // get the inputs
  auto lat = pro.get_input<double>(0);
  auto lon = pro.get_input<double>(1);
  auto el = pro.get_input<double>(2);
  std::string lvcs_name = pro.get_input<std::string>(3);

  // create lvcs
  vpgl_lvcs_sptr lvcs = new  vpgl_lvcs(lat, lon, el, vpgl_lvcs::str_to_enum(lvcs_name.c_str()), vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  int i=0;
  pro.set_output_val<vpgl_lvcs_sptr>(i++, lvcs);
  return true;

}
