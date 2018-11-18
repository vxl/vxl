// This is brl/bpro/core/vil_pro/processes/vil_save_image_view_binary_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view_base.h>
#include <vil/io/vil_io_image_view_base.h>

//: Constructor
bool vil_save_image_view_binary_process_cons(bprb_func_process& pro)
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
bool vil_save_image_view_binary_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_rgbi_to_grey_process: The input number should be 1" << std::endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);

  //Retrieve filename from input
  std::string image_filename = pro.get_input<std::string>(i++);

  vsl_b_ofstream os(image_filename);
  vsl_b_write(os, img);
  os.close();

  return true;
}
