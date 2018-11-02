// This is brl/bpro/core/vpgl_pro/processes/vpgl_save_perspective_camera_vrml_process.cxx
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
#include <vpgl/vpgl_perspective_camera.h>
#include <vsl/vsl_binary_io.h>

//: initialization
bool vpgl_save_perspective_camera_vrml_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs:
  //input[0]: the camera
  //input[1]: the filename
  //input[2]: radius of the sphere that will represent camera center in the output file
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("float");
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_save_perspective_camera_vrml_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 3) {
    std::cout << "vpgl_save_perspective_camera_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  std::string vrml_filename = pro.get_input<std::string>(1);
  auto radius = pro.get_input<float>(2);
  auto *cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.as_pointer());

  if (!cam) {
    std::cerr << "error: could not convert camera input to a vpgl_perspective_camera\n";
    return false;
  }

  std::ofstream os(vrml_filename.c_str());
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
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("float");
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_save_perspective_cameras_vrml_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 3) {
    std::cout << "vpgl_save_perspective_camera_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  std::string cam_dir = pro.get_input<std::string>(0);

  //populate vector of cameras
  //: returns a list of cameras from specified directory
  std::vector<vpgl_perspective_camera<double> > cams = cameras_from_directory(cam_dir, 0.0);

    std::string vrml_filename = pro.get_input<std::string>(1);
  auto radius = pro.get_input<float>(2);


  std::ofstream os(vrml_filename.c_str());
  os  << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n";
  for (const auto & cam : cams)
    vrml_write(os, cam, (double)radius);
  os.close();
  return true;
}
