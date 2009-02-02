// This is brl/bpro/core/vpgl_pro/processes/vpgl_save_perspective_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vnl/vnl_matrix_fixed.h>

//: initialization
bool vpgl_save_perspective_camera_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  //input[0]: the camera
  //input[1]: the filename
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr"); 
  input_types.push_back("vcl_string"); 
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_save_perspective_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    vcl_cout << "vpgl_save_perspective_camera_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  vcl_string camera_filename = pro.get_input<vcl_string>(1);
  vpgl_perspective_camera<double> *cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.as_pointer());

  if (!cam) {
    vcl_cerr << "error: could not convert camera input to a vpgl_perspective_camera" << vcl_endl;
    return false;
  }

  // write matrices to the text file.
  vcl_ofstream ofs(camera_filename.c_str());
  if (!ofs.is_open()) {
    vcl_cerr << "Failed to open file " << camera_filename << vcl_endl;
    return false;
  }
  ofs << *cam;
  ofs.close();

  return true;
}

