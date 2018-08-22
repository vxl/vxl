// This is brl/bpro/core/vpgl_pro/processes/vpgl_print_rational_camera_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_compiler.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: initialization
bool vpgl_print_rational_camera_process_cons(bprb_func_process& pro)
{
  // this process takes one input: the filename
  std::vector<std::string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  // this process has no output
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool vpgl_print_rational_camera_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!\n";
    return false;
  }
  // get input
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);

  vpgl_local_rational_camera<double>* cam = dynamic_cast<vpgl_local_rational_camera<double>*>(camera.as_pointer());
  if (!cam) {
    // try rational camera
    vpgl_rational_camera<double>* cam2 = dynamic_cast<vpgl_rational_camera<double>*>(camera.as_pointer());
    if (!cam2) {
      std::cerr << pro.name() << "error: could not convert camera input to a vpgl_rational_camera or local rational camera\n";
      return false;
    }
    cam2->print(std::cout);
  }
  else {
    cam->print(std::cout);
  }

  return true;
}