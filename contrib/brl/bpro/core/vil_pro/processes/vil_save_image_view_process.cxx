// This is brl/bpro/core/vil_pro/processes/vil_save_image_view_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_save.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_save_image_view_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  // input(0): the vil_image_view_base_sptr
  // input(1): the filename to save to
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vcl_string");
  return pro.set_input_types(input_types);

  //this process has no outputs
}


//: Execute the process
bool vil_save_image_view_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_save_image_view_process: The input number should be 2" << std::endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);

  //Retrieve filename from input
  std::string image_filename = pro.get_input<std::string>(i++);

  bool result = vil_save(*img,image_filename.c_str());

  if ( !result ) {
    std::cerr << "Failed to save image to" << image_filename << std::endl;
    return false;
  }

  return true;
}
