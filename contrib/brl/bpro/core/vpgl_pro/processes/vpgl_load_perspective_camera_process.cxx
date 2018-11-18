// This is brl/bpro/core/vpgl_pro/processes/vpgl_load_perspective_camera_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_file.h>

//: Init function
bool vpgl_load_perspective_camera_process_cons(bprb_func_process& pro)
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
bool vpgl_load_perspective_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    std::cout << "lvpgl_load_perspective_camera_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  std::string camera_filename = pro.get_input<std::string>(0);

  // read projection matrix from the file.
  std::ifstream ifs(camera_filename.c_str());
  if (!ifs.is_open()) {
    std::cerr << "Failed to open file " << camera_filename << '\n';
    return false;
  }
  auto* pcam =new vpgl_perspective_camera<double>;
  std::string ext = vul_file_extension(camera_filename);
  if (ext == ".vsl") // binary form
  {
    vsl_b_ifstream bp_in(camera_filename.c_str());
    vsl_b_read(bp_in, *pcam);
    bp_in.close();
  }
  else {
   ifs >> *pcam;
  }
  pro.set_output_val<vpgl_camera_double_sptr>(0, pcam);

  return true;
}

// create camera from kml parameters
bool vpgl_load_perspective_camera_from_kml_file_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(3);
  input_types[0] = "unsigned";// ni
  input_types[1] = "unsigned";// nj
  input_types[2] = "vcl_string";  // file name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types(4);
  output_types[0] = "vpgl_camera_double_sptr";  // output camera
  output_types[1] = "double";  // longitude, the returned camera is at 0,0,altitude however return the read lat, lon in case it is used by other processes
  output_types[2] = "double";  // latitude
  output_types[3] = "double";  // altitude
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

#include <bkml/bkml_parser.h>
#include <bpgl/bpgl_camera_utils.h>

//: Execute the process
bool vpgl_load_perspective_camera_from_kml_file_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_create_perspective_camera_process4: Invalid inputs\n";
    return false;
  }
  // get the inputs
  auto ni = pro.get_input<unsigned>(0);
  auto nj = pro.get_input<unsigned>(1);
  std::string name = pro.get_input<std::string>(2);

  auto* parser = new bkml_parser();
  std::FILE* xmlFile = std::fopen(name.c_str(), "r");
  if (!xmlFile) {
    std::cerr << name.c_str() << " error on opening\n";
    delete parser;
    return false;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';

    delete parser;
    return false;
  }
  vpgl_perspective_camera<double> out_cam =
    bpgl_camera_utils::camera_from_kml((double)ni, (double)nj, parser->right_fov_, parser->top_fov_, parser->altitude_, parser->heading_, parser->tilt_, parser->roll_);

  auto* ncam = new vpgl_perspective_camera<double>(out_cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);
  pro.set_output_val<double>(1, parser->longitude_);
  pro.set_output_val<double>(2, parser->latitude_);
  pro.set_output_val<double>(3, parser->altitude_);
  return true;
}
