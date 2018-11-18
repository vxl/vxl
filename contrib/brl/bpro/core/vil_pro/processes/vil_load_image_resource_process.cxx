// This is brl/bpro/core/vil_pro/processes/vil_load_image_resource_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>

//: Constructor
bool vil_load_image_resource_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_resource_sptr");
  output_types.emplace_back("unsigned");  // ni of the image
  output_types.emplace_back("unsigned");  // nj of the image
  output_types.emplace_back("unsigned"); //nplanes of the image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_load_image_resource_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_load_image_view_binary_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve filename from input
  std::string image_filename = pro.get_input<std::string>(i++);

  vil_image_resource_sptr loaded_image = vil_load_image_resource(image_filename.c_str());
  if ( !loaded_image ) {
    std::cerr << "Failed to load image file: " << image_filename << std::endl;
    return false;
  }

  i=0;
  pro.set_output_val<vil_image_resource_sptr>(i++, loaded_image);
  pro.set_output_val<unsigned>(i++, loaded_image->ni());
  pro.set_output_val<unsigned>(i++, loaded_image->nj());
  pro.set_output_val<unsigned>(i++, loaded_image->nplanes());
  return true;
}
