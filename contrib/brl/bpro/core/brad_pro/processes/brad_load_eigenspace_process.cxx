// This is brl/bpro/core/brad_pro/processes/brad_load_eigenspace_process.cxx
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
bool brad_load_eigenspace_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); //eigenspace path
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //no output
  std::vector<std::string> output_types;
  output_types.emplace_back("brad_eigenspace_sptr"); //eigenspace ptr
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_load_eigenspace_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "brad_load_eigenspace_process: The input number should be 1" << std::endl;
    return false;
  }
  int i = 0;
  std::string path = pro.get_input<std::string>(i++);
  vsl_b_ifstream is(path.c_str());
  if (!is) {
    std::cout << "in load_eigenspace_process, couldn't open input file stream\n";
    return false;
  }
  brad_eigenspace_sptr espace;
  vsl_b_read(is, espace);
  if (!espace){
    std::cout << "in load_eigenspace_process, null eigenspace pointer\n";
    return false;
  }
  pro.set_output_val<brad_eigenspace_sptr>(0, espace);
  return true;
}
