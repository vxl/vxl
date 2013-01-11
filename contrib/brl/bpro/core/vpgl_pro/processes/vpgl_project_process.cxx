// This is brl/bpro/core/vpgl_pro/processes/vpgl_project_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vsl/vsl_binary_io.h>

//: initialization
bool vpgl_project_process_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs:
  //input[0]: the camera
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  input_types.push_back("float");
  input_types.push_back("float");
  input_types.push_back("float");
  pro.set_input_types(input_types);

  // this process has two outputs:
  vcl_vector<vcl_string> output_types;
  output_types.push_back("float");
  output_types.push_back("float");
  pro.set_output_types(output_types);

  return true;
}

//: Execute the process
bool vpgl_project_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 4) {
    vcl_cout << "vpgl_project_process: The number of inputs should be 4" << vcl_endl;
    return false;
  }

  // get the inputs
  int i=0;
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i);

  float x=pro.get_input<float>(++i);
  float y=pro.get_input<float>(++i);
  float z=pro.get_input<float>(++i);

  double u,v;
  camera->project(x,y,z,u,v);

  pro.set_output_val<float>(0, (float)u);
  pro.set_output_val<float>(1, (float)v);

  return true;
}

