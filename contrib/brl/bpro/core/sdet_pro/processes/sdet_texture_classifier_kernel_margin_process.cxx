// This is brl/bpro/core/sdet_pro/kernel_margin_processes/sdet_texture_classifier_kernel_margin_process.cxx
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_texture_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>
#include <vil/vil_image_view.h>
//: initialize input and output types
bool sdet_texture_classifier_kernel_margin_process_cons(bprb_func_process& pro)
{
  // kernel_margin_process takes 3 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); //texton dictionary path
  if (!pro.set_input_types(input_types))
    return false;

  // kernel_margin_process has 1 output:
  // output[0]: kernel margin
  vcl_vector<vcl_string> output_types;
  output_types.push_back("unsigned");
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_classifier_kernel_margin_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier kernel_margin_process inputs are not valid "<< vcl_endl;
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
  pro.set_output_val<unsigned>(0, margin);
  return true;
}
