// This is brl/bpro/core/vpgl_pro/processes/vpgl_project_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_file.h>

//: initialization
bool vpgl_project_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  //input[0]: the camera
  //input[1]: the filename
  //input[2]: radius of the sphere that will represent camera center in the output file
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  input_types.push_back("float");
  input_types.push_back("float");
  input_types.push_back("float");
  pro.set_input_types(input_types);

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
    vcl_cout << "vpgl_save_perspective_camera_process: The input number should be 1" << vcl_endl;
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

