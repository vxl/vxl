// This is brl/bpro/core/vpgl_pro/processes/vpgl_load_proj_camera_process.cxx
#include <iostream>
#include <fstream>
#include <iomanip>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/io/vpgl_io_proj_camera.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vul/vul_file.h>

//: Constructor
bool vpgl_load_proj_camera_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename, and one output: the camera
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // label image
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}


//: Execute the process
bool vpgl_load_proj_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    std::cout << "lvpgl_load_proj_camera_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  std::string camera_filename = pro.get_input<std::string>(0);

  std::string ext = vul_file::extension(camera_filename);
  if (ext == ".vsl") { // load binary
    auto* procamp = new vpgl_proj_camera<double>();
    vsl_b_ifstream ins(camera_filename.c_str());
    vsl_b_read(ins, *procamp);
    ins.close();
    vpgl_camera_double_sptr procam = procamp;
    pro.set_output_val<vpgl_camera_double_sptr>(0, procam);
    return true;
  }

  // read projection matrix from the file.
  std::ifstream ifs(camera_filename.c_str());
  if (!ifs.is_open()) {
    std::cerr << "Failed to open file " << camera_filename << '\n';
    return false;
  }
  vnl_matrix_fixed<double,3,4> projection_matrix;
  ifs >> projection_matrix;

  ifs >> std::skipws;
  if(!ifs.eof())
    std::cout << "Warning: More text left to parse.\n";

  vpgl_camera_double_sptr procam = new vpgl_proj_camera<double>(projection_matrix);

  pro.set_output_val<vpgl_camera_double_sptr>(0, procam);

  return true;
}
