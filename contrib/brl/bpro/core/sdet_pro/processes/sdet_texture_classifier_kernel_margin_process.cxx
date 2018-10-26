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
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); //texton dictionary path
  pro.set_input_types(input_types);
  // kernel_margin_process has 1 output:
  std::vector<std::string> output_types;
  output_types.emplace_back("int");
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_classifier_kernel_margin_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier kernel_margin_process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  std::string texton_dict_path = pro.get_input<std::string>(0);

  sdet_texture_classifier_params tcp;
  sdet_texture_classifier tc(tcp);
 //parameter defaults will be potentially overwritten by loading the dictionary
  tc.load_dictionary(texton_dict_path);
  unsigned margin = tc.max_filter_radius();
  std::cout << "output margin = " << margin << '\n';
  pro.set_output_val<int>(0, (int)margin);
  return true;
}
