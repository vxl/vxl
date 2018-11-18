// This is brl/bpro/core/vpgl_pro/processes/vpgl_create_local_rational_camera_process.cxx
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
#include <vpgl/vpgl_lvcs.h>

//: initialization
bool vpgl_create_local_rational_camera_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs and has one output
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vcl_string");
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_create_local_rational_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 2) {
    std::cout << "vpgl_create_local_rational_camera_process: The number of inputs should be 2" << std::endl;
    return false;
  }

  // get the inputs
  std::string camera_filename = pro.get_input<std::string>(0);
  std::string lvcs_filename = pro.get_input<std::string>(1);

  //vpgl_rational_camera<double>* ratcam = read_local_rational_camera<double>(camera_filename);

  //if ( ratcam ) {
  //  std::cerr << "Error: rational camera is already local! Use load_rational_camera\n";
  //  return false;
  //}

  vpgl_rational_camera<double>* ratcam = read_rational_camera<double>(camera_filename);
  if ( !ratcam ) {
    std::cerr << "Failed to load rational camera from file" << camera_filename << '\n';
    return false;
  }

  if (dynamic_cast<vpgl_local_rational_camera<double>*>(ratcam)) {
    std::cerr << "Error: rational camera is already local! Use load_rational_camera\n";
    return false;
  }

  vpgl_lvcs lvcs;
  std::ifstream ifs(lvcs_filename.c_str());
  if (!ifs.good()) {
    std::cerr << "Error opening lvcs filename " << lvcs_filename << '\n';
    return false;
  }

  lvcs.read(ifs);
  vpgl_camera_double_sptr local_ratcam = new vpgl_local_rational_camera<double>(lvcs, *ratcam);

  pro.set_output_val<vpgl_camera_double_sptr>(0, local_ratcam);

  return true;
}
