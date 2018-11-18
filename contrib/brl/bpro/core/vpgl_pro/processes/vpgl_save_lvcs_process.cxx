// This is brl/bpro/core/vpgl_pro/processes/vpgl_save_lvcs_process.cxx
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

//: Save a lvcs
//: initialization
bool vpgl_save_lvcs_process_cons(bprb_func_process& pro)
{
  // this process takes two inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_lvcs_sptr");
  input_types.emplace_back("vcl_string");
  // this process has no output
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

//:execute the process
bool vpgl_save_lvcs_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong inputs!!!\n";
    return false;
  }
  // get input
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(0);
  std::string filename = pro.get_input<std::string>(1);

  if (!lvcs) {
    std::cerr << pro.name() << ": input lvcs is empty!!\n";
    return false;
  }
  // save to file
  std::ofstream ofs(filename.c_str());
  if (!ofs.good()) {
    std::cerr << pro.name() << ": Error opening lvcs file: " << filename << "!!\n";
    return false;
  }

  lvcs->write(ofs);
  ofs.close();
  return true;
}


//: Create a lvcs and save it into lvcs file
//: initialization
bool vpgl_create_and_save_lvcs_process_cons(bprb_func_process& pro)
{
  //this process takes four inputs:
  // 0: (float)  latitude
  // 1: (float)  longitude
  // 2: (float)  elevation
  // 3: (string) lvcs cs name
  // 4: (string) lvcs filename to save
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("float");
  input_types.emplace_back("float");
  input_types.emplace_back("float");
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // process has no outputs
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_create_and_save_lvcs_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cout << pro.name() << ": The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }

  // get the inputs
  auto lat = pro.get_input<float>(0);
  auto lon = pro.get_input<float>(1);
  auto el  = pro.get_input<float>(2);
  std::string cs_name = pro.get_input<std::string>(3);
  std::string lvcs_filename = pro.get_input<std::string>(4);

  // create lvcs
  vpgl_lvcs lvcs(lat, lon, el, vpgl_lvcs::str_to_enum(cs_name.c_str()), vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  // save to file
  std::ofstream ofs(lvcs_filename.c_str());
  if (!ofs.good()) {
    std::cerr << "Error opening lvcs filename " << lvcs_filename << '\n';
    return false;
  }

  lvcs.write(ofs);

  return true;
}
