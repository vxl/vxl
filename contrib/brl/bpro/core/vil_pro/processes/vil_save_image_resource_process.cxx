// This is brl/bpro/core/vil_pro/processes/vil_save_image_resource_process.cxx
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
#include <vil/vil_image_resource.h>

//: Constructor
bool vil_save_image_resource_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_resource_sptr");
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_save_image_resource_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_save_image_resource_binary_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_resource_sptr resc = pro.get_input<vil_image_resource_sptr>(i++);
  std::string image_filename = pro.get_input<std::string>(i);
  if(!resc){
    std::cout << "null input resource " << std::endl;
    return false;
  }

  if(!vil_save_image_resource(resc, image_filename.c_str())){
    std::cout << " save image resource failed " << std::endl;
    return false;
  }
  return true;
}
