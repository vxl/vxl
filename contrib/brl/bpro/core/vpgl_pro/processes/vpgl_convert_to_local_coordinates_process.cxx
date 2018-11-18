// This is brl/bpro/core/vpgl_pro/processes/vpgl_convert_to_local_coordinates_process.cxx
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
bool vpgl_convert_to_local_coordinates_process_cons(bprb_func_process& pro)
{
  //this process takes four inputs:
  // 0: (string) lvcs filename
  // 1: (float)  latitude
  // 2: (float)  longitude
  // 3: (float)  elevation
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("float");
  input_types.emplace_back("float");
  input_types.emplace_back("float");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // outputs x,y,z in local coordinates
  // 0: (float) x
  // 1: (float) y
  // 2: (float) z
  std::vector<std::string> output_types;
  output_types.emplace_back("float");
  output_types.emplace_back("float");
  output_types.emplace_back("float");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;

}

//: Execute the process
bool vpgl_convert_to_local_coordinates_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != pro.input_types().size()) {
    std::cout << "vpgl_convert_to_local_coordinates_process: The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }

  // get the inputs
  std::string lvcs_filename = pro.get_input<std::string>(0);
  auto lat = pro.get_input<float>(1);
  auto lon = pro.get_input<float>(2);
  auto el = pro.get_input<float>(3);

  vpgl_lvcs lvcs;
  std::ifstream ifs(lvcs_filename.c_str());
  if(!ifs.good()) {
    std::cerr << "Error opening lvcs filename " << lvcs_filename << std::endl;
    return false;
  }

  lvcs.read(ifs);

  double x,y,z;
  lvcs.global_to_local(lon,lat,el, vpgl_lvcs::wgs84, x, y, z, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  pro.set_output_val<double>(0, (double)x);
  pro.set_output_val<double>(1, (double)y);
  pro.set_output_val<double>(2, (double)z);

  return true;
}

//: initialization
bool vpgl_convert_to_local_coordinates_process2_cons(bprb_func_process& pro)
{
  //this process takes four inputs:
  // 0: lvcs
  // 1: (float)  latitude
  // 2: (float)  longitude
  // 3: (float)  elevation
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_lvcs_sptr");
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // outputs x,y,z in local coordinates
  // 0: (double) x
  // 1: (double) y
  // 2: (double) z
  std::vector<std::string> output_types;
  output_types.emplace_back("double");
  output_types.emplace_back("double");
  output_types.emplace_back("double");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;

}

//: Execute the process
bool vpgl_convert_to_local_coordinates_process2(bprb_func_process& pro)
{
  if (pro.n_inputs() != pro.input_types().size()) {
    std::cout << "vpgl_convert_to_local_coordinates_process: The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }

  // get the inputs
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(0);
  auto lat = pro.get_input<double>(1);
  auto lon = pro.get_input<double>(2);
  auto el = pro.get_input<double>(3);

  double x,y,z;
  lvcs->global_to_local(lon,lat,el, vpgl_lvcs::wgs84, x, y, z, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  pro.set_output_val<double>(0, (double)x);
  pro.set_output_val<double>(1, (double)y);
  pro.set_output_val<double>(2, (double)z);

  return true;
}
