// This is brl/bpro/core/sdet_pro/processes/sdet_texture_classifier_kernel_margin_process.cxx
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
//:
// \file

#include <sdet/sdet_texture_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>

//: initialize input and output types
bool sdet_texture_classifier_kernel_margin_process_cons(bprb_func_process& pro)
{
  // kernel_margin_process takes 1 input:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); //texton dictionary path
  pro.set_input_types(input_types);
  // kernel_margin_process has 1 output:
  vcl_vector<vcl_string> output_types;
  output_types.push_back("int");
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_classifier_kernel_margin_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier kernel_margin_process inputs are not valid"<< vcl_endl;
    return false;
  }
  // get inputs
  vcl_string texton_dict_path = pro.get_input<vcl_string>(0);

  sdet_texture_classifier_params tcp;
  sdet_texture_classifier tc(tcp);
 //parameter defaults will be potentially overwritten by loading the dictionary
  tc.load_dictionary(texton_dict_path);
  unsigned margin = tc.max_filter_radius();
  vcl_cout << "output margin = " << margin << '\n';
  pro.set_output_val<int>(0, (int)margin);
  return true;
}
