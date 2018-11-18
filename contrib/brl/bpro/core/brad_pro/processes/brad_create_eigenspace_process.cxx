// This is brl/bpro/core/brad_pro/processes/brad_create_eigenspace_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <brad/brad_eigenspace.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool brad_create_eigenspace_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  std::vector<std::string> input_types(6);
  input_types[0]= "vcl_string"; // feature vector type
  input_types[1]= "float";      // intensity hist max
  input_types[2]= "float";      // gradient hist max
  input_types[3]= "unsigned";   // number of bins
  input_types[4]= "unsigned";   // number of cols in block - nib
  input_types[5]= "unsigned";   // number of cols in block - njb
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brad_eigenspace_sptr"); //eigenspace
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_create_eigenspace_process(bprb_func_process& pro)
{
#if 0
  // Sanity check
  if (pro.n_inputs()< 6) {
    std::cout << "brad_create_eigenspace_process: The input number should be 6" << std::endl;
    return false;
  }
#endif
  int i = 0;
  std::string feature_vector_type = pro.get_input<std::string>(i++);
  auto max_int = pro.get_input<float>(i++);
  auto max_grad = pro.get_input<float>(i++);
  auto nbins = pro.get_input<unsigned>(i++);
  auto nib = pro.get_input<unsigned>(i++);
  auto njb = pro.get_input<unsigned>(i++);

  brad_eigenspace_sptr eptr = nullptr;
  CAST_CREATE_EIGENSPACE(feature_vector_type, nbins, max_int, max_grad)
  pro.set_output_val<brad_eigenspace_sptr>(0, eptr);
  return true;
}
