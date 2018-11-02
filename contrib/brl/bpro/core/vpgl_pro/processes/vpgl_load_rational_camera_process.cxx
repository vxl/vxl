// This is brl/bpro/core/vpgl_pro/processes/vpgl_load_rational_camera_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: initialization
bool vpgl_load_rational_camera_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;

}

//: Execute the process
bool vpgl_load_rational_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    std::cout << "lvpgl_load_rational_camera_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  std::string camera_filename = pro.get_input<std::string>(0);

  vpgl_camera_double_sptr ratcam = read_local_rational_camera<double>(camera_filename);

  if ( !ratcam.as_pointer() ) {
    //std::cerr << "Rational camera isn't local... trying global" << std::endl;
    ratcam = read_rational_camera<double>(camera_filename);

  }

  if ( !ratcam.as_pointer() ) {
    std::cerr << "Failed to load rational camera from file" << camera_filename << std::endl;
    return false;
  }

  pro.set_output_val<vpgl_camera_double_sptr>(0, ratcam);

  return true;
}

//: initialization
bool vpgl_load_rational_camera_from_txt_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;

}

//: Execute the process
bool vpgl_load_rational_camera_from_txt_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    std::cout << "lvpgl_load_rational_camera_from_txt_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  std::string camera_filename = pro.get_input<std::string>(0);

  vpgl_camera_double_sptr ratcam = read_rational_camera_from_txt<double>(camera_filename);

  if ( !ratcam.as_pointer() ) {
    std::cerr << "Failed to load rational camera from file" << camera_filename << std::endl;
    return false;
  }

  pro.set_output_val<vpgl_camera_double_sptr>(0, ratcam);

  return true;
}
