// This is brl/bpro/core/brad_pro/processes/brad_describe_eigenspace_process.cxx
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
bool brad_describe_eigenspace_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brad_eigenspace_sptr"); //eigenspace
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //no output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_describe_eigenspace_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "brad_describe_eigenspace_process: The input number should be 1" << std::endl;
    return false;
  }
  brad_eigenspace_sptr es_ptr = pro.get_input<brad_eigenspace_sptr>(0);
  if (!es_ptr){
    std::cout << "in describe_eigenspace_process, null eigenspace pointer\n";
    return false;
  }
  std::cout << "\n>>>>brad_eigenspace description<<<<\n" << std::flush;
  CAST_CALL_EIGENSPACE(es_ptr, ep->print(), "");
  std::cout << '\n';
  return true;
}
