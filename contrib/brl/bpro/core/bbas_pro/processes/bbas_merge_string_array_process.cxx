// This is brl/bpro/core/bbas_pro/processes/bbas_merge_string_array_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>

//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
namespace bbas_core_merge_string_array
{
  std::vector<std::string> strings;
}
//: Constructor
bool bbas_merge_string_array_process_cons(bprb_func_process& pro)
{
  using namespace bbas_core_merge_string_array;
  // inputs
  std::vector<std::string> input_types;
  input_types.emplace_back("bbas_1d_array_string_sptr"); //1st string
  //outputs
  std::vector<std::string> output_types;
  output_types.emplace_back("bbas_1d_array_string_sptr"); //2nd string

  return pro.set_input_types(input_types) &&
         pro.set_output_types(output_types);
}

//: Execute the process
bool bbas_merge_string_array_process(bprb_func_process& pro)
{
  using namespace bbas_core_merge_string_array;
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "brad_merge_string_array_process: The input number should be 1" << std::endl;
    return false;
  }
  bbas_1d_array_string_sptr sa0 = pro.get_input<bbas_1d_array_string_sptr>(0);
  bbas_1d_array_string_sptr sa1 = pro.get_input<bbas_1d_array_string_sptr>(1);
  unsigned n0 = (*sa0).data_array.size();
  unsigned n1 = (*sa1).data_array.size();
  bbas_1d_array_string_sptr out = new bbas_1d_array_string(n0+n1);
  for (unsigned i = 0; i<n0; ++i)
    (out->data_array)[i]=(sa0->data_array)[i];
  for (unsigned i = 0; i<n1; ++i)
    (out->data_array)[n0+i]=(sa1->data_array)[i];
  pro.set_output_val<bbas_1d_array_string_sptr>(0, out);
  return true;
}
