// This is brl/bpro/core/vpgl_pro/processes/vpgl_save_perspective_camera_process.cxx
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
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_file.h>

//: initialization
bool vpgl_save_perspective_camera_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  //input[0]: the camera
  //input[1]: the filename
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("vcl_string");
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_save_perspective_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    std::cout << "vpgl_save_perspective_camera_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  std::string camera_filename = pro.get_input<std::string>(1);
  auto *cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.as_pointer());

  if (!cam) {
    std::cerr << "error: could not convert camera input to a vpgl_perspective_camera\n";
    return false;
  }

  std::string ext = vul_file_extension(camera_filename);
  if (ext == ".vsl") // binary form
  {
    vsl_b_ofstream bp_out(camera_filename.c_str());
    vsl_b_write(bp_out, *cam);
    bp_out.close();
  }
  else {
    // write matrices to the text file.
    std::ofstream ofs(camera_filename.c_str());
    if (!ofs.is_open()) {
      std::cerr << "Failed to open file " << camera_filename << '\n';
      return false;
    }
    ofs << *cam;
    ofs.close();
  }
  return true;
}

//: save projective camera's matrix with get_matrix()
bool vpgl_save_projective_camera_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  //input[0]: the camera
  //input[1]: the filename
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("vcl_string");
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_save_projective_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    std::cout << "vpgl_save_perspective_camera_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  std::string camera_filename = pro.get_input<std::string>(1);
  auto *cam = dynamic_cast<vpgl_proj_camera<double>*>(camera.as_pointer());

  if (!cam) {
    std::cerr << "error: could not convert camera input to a vpgl_proj_camera\n";
    return false;
  }

  std::ofstream ofs(camera_filename.c_str());
  if (!ofs.is_open()) {
    std::cerr << "Failed to open file " << camera_filename << '\n';
    return false;
  }
  ofs << cam->get_matrix();
  ofs.close();
  return true;
}
