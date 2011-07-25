// This is brl/bpro/core/bbas_pro/processes/bbas_string_array_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <vcl_fstream.h>

namespace bbas_core_string_array
{
  vcl_vector<vcl_string> strings;
}

//: Constructor
bool bbas_string_array_process_cons(bprb_func_process& pro)
{
  using namespace bbas_core_string_array;
  // inputs
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); //input string
  //outputs
  vcl_vector<vcl_string> output_types;
  output_types.push_back("bbas_1d_array_string_sptr"); //string array

  return pro.set_input_types(input_types) &&
         pro.set_output_types(output_types);
}

//: Execute the process
bool bbas_string_array_process(bprb_func_process& pro)
{
  using namespace bbas_core_string_array;
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "brad_string_array_process: The input number should be 1" << vcl_endl;
    return false;
  }
  vcl_string path = pro.get_input<vcl_string>(0);
  strings.push_back(path);
  return true;
}

bool bbas_string_array_process_finish(bprb_func_process& pro)
{
  using namespace bbas_core_string_array;
  unsigned n = strings.size();
  bbas_1d_array_string_sptr ar = new bbas_1d_array_string(n);
  for (unsigned i = 0; i<n; ++i)
    (*ar).data_array[i]=strings[i];
  pro.set_output_val<bbas_1d_array_string_sptr>(0, ar);
  strings.clear();
  return true;
}
