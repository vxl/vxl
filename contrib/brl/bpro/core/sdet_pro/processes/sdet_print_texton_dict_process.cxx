// This is brl/bpro/core/sdet_pro/processes/sdet_print_texton_dict_process.cxx
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_texture_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>

//: initialize input and output types
bool sdet_print_texton_dict_process_cons(bprb_func_process& pro)
{
  // process takes 3 inputs, no outputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); //classifier
  input_types.emplace_back("vcl_string"); //texton dictionary
  input_types.emplace_back("vcl_string");  // print mode
  // "histograms" , "textons", "inter_probs", "inter_dist"
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
     &&  pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_print_texton_dict_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "print texton dictionary process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr =
    pro.get_input<sdet_texture_classifier_sptr>(0);
  auto* tcp =
    static_cast<sdet_texture_classifier_params*>(tc_ptr.ptr());
  sdet_texture_classifier tc(*tcp);

  std::string texton_dict_path = pro.get_input<std::string>(1);
  tc.load_dictionary(texton_dict_path);

  std::string print_mode = pro.get_input<std::string>(2);
  if (print_mode == "histograms") {
    tc.print_category_histograms();
    return true;
  }
  else if (print_mode == "textons") {
    tc.print_dictionary();
    return true;
  }
  else if (print_mode == "inter_probs") {
    tc.print_interclass_probs();
    return true;
  }
  else if (print_mode == "inter_dist") {
    tc.print_distances();
    return true;
  }
  else {
    std::cout << "unrecognized print mode\n";
    return false;
  }
}
