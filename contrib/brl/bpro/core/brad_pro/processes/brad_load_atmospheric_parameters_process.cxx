// This is brl/bpro/core/brad_pro/processes/brad_load_atmospheric_parameters_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <brad/brad_atmospheric_parameters.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool brad_load_atmospheric_parameters_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brad_atmospheric_parameters_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_load_atmospheric_parameters_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 1) {
    std::cout << "brad_load_atmospheric_parameters_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the input
  std::string filename = pro.get_input<std::string>(0);

  brad_atmospheric_parameters_sptr metadata = new brad_atmospheric_parameters();

  std::ifstream ifs(filename.c_str());
  if (!ifs.is_open()) {
    std::cout << "in load_atmospheric_parameters_process, couldn't open file: " << filename << std::endl;
    return false;
  }

  ifs >> *metadata;

  ifs.close();

  pro.set_output_val<brad_atmospheric_parameters_sptr>(0,metadata);

  return true;
}
