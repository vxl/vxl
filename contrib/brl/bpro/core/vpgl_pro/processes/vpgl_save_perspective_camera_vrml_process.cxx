// This is brl/bpro/core/vpgl_pro/processes/vpgl_save_perspective_camera_vrml_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vsl/vsl_binary_io.h>

//: initialization
bool vpgl_save_perspective_camera_vrml_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs:
  //input[0]: the camera
  //input[1]: the filename
  //input[2]: radius of the sphere that will represent camera center in the output file
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  input_types.push_back("vcl_string");
  input_types.push_back("float");
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_save_perspective_camera_vrml_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 3) {
    vcl_cout << "vpgl_save_perspective_camera_process: The number of inputs should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  vcl_string vrml_filename = pro.get_input<vcl_string>(1);
  float radius = pro.get_input<float>(2);
  vpgl_perspective_camera<double> *cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.as_pointer());

  if (!cam) {
    vcl_cerr << "error: could not convert camera input to a vpgl_perspective_camera\n";
    return false;
  }

  vcl_ofstream os(vrml_filename.c_str());
  os  << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n";
  vrml_write(os, *cam, (double)radius);
  os.close();
  return true;
}

//: initialization
bool vpgl_save_perspective_cameras_vrml_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs:
  //input[0]: the camera
  //input[1]: the filename
  //input[2]: radius of the sphere that will represent camera center in the output file
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  input_types.push_back("vcl_string");
  input_types.push_back("float");
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_save_perspective_cameras_vrml_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 3) {
    vcl_cout << "vpgl_save_perspective_camera_process: The number of inputs should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string cam_dir = pro.get_input<vcl_string>(0);

  //populate vector of cameras
  //: returns a list of cameras from specified directory
  vcl_vector<vpgl_perspective_camera<double> > cams = cameras_from_directory(cam_dir, 0.0);

    vcl_string vrml_filename = pro.get_input<vcl_string>(1);
  float radius = pro.get_input<float>(2);


  vcl_ofstream os(vrml_filename.c_str());
  os  << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n";
  for (unsigned i = 0 ; i < cams.size(); ++i)
    vrml_write(os, cams[i], (double)radius);
  os.close();
  return true;
}
