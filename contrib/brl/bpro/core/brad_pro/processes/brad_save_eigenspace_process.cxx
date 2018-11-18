// This is brl/bpro/core/brad_pro/processes/brad_save_eigenspace_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <brad/brad_eigenspace.h>
#include <vsl/vsl_binary_io.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool brad_save_eigenspace_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brad_eigenspace_sptr"); //eigenspace
  input_types.emplace_back("vcl_string"); //eigenspace path
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //no output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_save_eigenspace_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "brad_save_eigenspace_process: The input number should be 1" << std::endl;
    return false;
  }
  int i = 0;
  brad_eigenspace_sptr espace = pro.get_input<brad_eigenspace_sptr>(i++);
  if (!espace){
    std::cout << "in save_eigenspace_process, null eigenspace pointer\n";
    return false;
  }

  std::string path = pro.get_input<std::string>(i++);
  vsl_b_ofstream os(path.c_str());
  if (!os) {
    std::cout << "in save_eigenspace_process, couldn't open output file stream\n";
    return false;
  }
  vsl_b_write(os, espace);
  return true;
}
