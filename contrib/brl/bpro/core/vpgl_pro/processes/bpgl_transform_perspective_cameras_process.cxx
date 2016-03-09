// This is brl/bpro/core/vpgl_pro/processes/bpgl_transform_perspective_cameras_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <iostream>
#include <vcl_compiler.h>
#include <iostream>
#include <fstream>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/algo/bpgl_transform_camera.h>

namespace bpgl_transform_perspective_cameras_process_globals
{
    const unsigned n_inputs_ = 3;
    const unsigned n_outputs_ = 0;
}

//: Init function
bool bpgl_transform_perspective_cameras_process_cons(bprb_func_process& pro)
{
  using namespace bpgl_transform_perspective_cameras_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // transformation file
  input_types_[1] = "vcl_string"; // Input cam dir
  input_types_[2] = "vcl_string"; // Output cam dir

  // process has 0 outputs
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

//: Execute the process
bool bpgl_transform_perspective_cameras_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "bpgl_transform_perspective_cameras_process: Invalid inputs\n";
    return false;
  }

  // get the inputs
  unsigned i=0;
  std::string xform_file = pro.get_input<std::string>(i++);
  std::string in_dir     = pro.get_input<std::string>(i++);
  std::string out_dir    = pro.get_input<std::string>(i++);
  std::cout<<"out_dir "<<out_dir<<std::endl;
  // check if input directory exists
  if (!vul_file::is_directory(in_dir.c_str()))
  {
    std::cout<<"Input Camera directory does not exist"<<std::endl;
    return false;
  }

  // check if output directory exists
  if (!vul_file::is_directory(out_dir.c_str()))
  {
    std::cout<<"Output Camera directory does not exist"<<std::endl;
    return false;
  }

  // read the xform file
  // * xaxis yaxis zaxis roation angle
  // * tx ty tz
  // * scale
  std::ifstream ifile(xform_file.c_str());
  if (!ifile)
  {
    std::cout<<"Cannot open Xform file"<<std::endl;
    return false;
  }

  double xr,yr,zr;
  double tx,ty,tz,scale;

  ifile>>xr>>yr>>zr;
  ifile>>tx>>ty>>tz;
  ifile>>scale;
  vnl_quaternion<double> q(xr,yr,zr);
  vgl_rotation_3d<double> R(q);
  vnl_vector_fixed<double, 3> t;
  t[0] = tx; t[1] = ty; t[2] = tz;
  // xform and save the xformed cams
  std::string glob_in_dir = in_dir + "/*.txt";
  for (vul_file_iterator fn = glob_in_dir.c_str(); fn; ++fn) {
    std::string f = fn();
    std::ifstream is(f.c_str());
    vpgl_perspective_camera<double> cam;
    is >> cam;
    is.close();
    std::string fname = vul_file::strip_directory(f.c_str());
    vpgl_perspective_camera<double> tcam = bpgl_transform_camera::transform_perspective_camera(cam, R, t, scale);
    std::string out_file = out_dir + "/"+ fname;
    std::ofstream os(out_file.c_str());
    os << tcam;
    os.close();
  }
  return true;
}
