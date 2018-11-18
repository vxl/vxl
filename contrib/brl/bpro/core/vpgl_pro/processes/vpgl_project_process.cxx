// This is brl/bpro/core/vpgl_pro/processes/vpgl_project_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vsl/vsl_binary_io.h>

//: initialization
bool vpgl_project_process_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs:
  //input[0]: the camera
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  pro.set_input_types(input_types);

  // this process has two outputs:
  std::vector<std::string> output_types;
  output_types.emplace_back("double");
  output_types.emplace_back("double");
  pro.set_output_types(output_types);

  return true;
}

//: Execute the process
bool vpgl_project_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 4) {
    std::cout << "vpgl_project_process: The number of inputs should be 4" << std::endl;
    return false;
  }

  // get the inputs
  int i=0;
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i);

  auto x=pro.get_input<double>(++i);
  auto y=pro.get_input<double>(++i);
  auto z=pro.get_input<double>(++i);

  double u,v;
  camera->project(x,y,z,u,v);

  pro.set_output_val<double>(0, (double)u);
  pro.set_output_val<double>(1, (double)v);

  return true;
}
