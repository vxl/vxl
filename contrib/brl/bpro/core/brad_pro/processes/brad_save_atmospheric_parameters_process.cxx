// This is brl/bpro/core/brad_pro/processes/brad_save_atmospheric_parameters_process.cxx
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
bool brad_save_atmospheric_parameters_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brad_atmospheric_parameters_sptr");
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types; // no outputs
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_save_atmospheric_parameters_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 2) {
    std::cout << "brad_save_atmospheric_parameters_process: The number of inputs should be 2" << std::endl;
    return false;
  }

  // get the inputs
  brad_atmospheric_parameters_sptr atm_params = pro.get_input<brad_atmospheric_parameters_sptr>(0);
  std::string out_file = pro.get_input<std::string>(1);

  //output file name
  std::ofstream os(out_file.c_str());
  if (!os.is_open()) {
    std::cout << "in save_atmospheric_parameters_process, couldn't open file : " << out_file << std::endl;
    return false;
  }

  os << *atm_params;

  os.close();

  return true;
}
