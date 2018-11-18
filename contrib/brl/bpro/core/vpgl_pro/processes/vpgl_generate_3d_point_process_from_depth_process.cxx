// This is brl/bpro/core/vpgl_pro/processes/vpgl_generate_3d_point_process_from_depth_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vsl/vsl_binary_io.h>

namespace vpgl_generate_3d_point_from_depth_process_globals
{
    constexpr unsigned n_inputs_ = 4;
    constexpr unsigned n_outputs_ = 3;
}
//: Init function
bool vpgl_generate_3d_point_from_depth_process_cons(bprb_func_process& pro)
{
    using namespace vpgl_generate_3d_point_from_depth_process_globals;

    //process takes 4 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "vpgl_camera_double_sptr";
    input_types_[1] = "float";
    input_types_[2] = "float";
    input_types_[3] = "float";

    // process has 3 outputs
    std::vector<std::string>  output_types_(n_outputs_);
    output_types_[0] = "float"; //X
    output_types_[1] = "float"; //Y
    output_types_[2] = "float"; //Z

    return pro.set_input_types(input_types_) &&
           pro.set_output_types(output_types_);
}

//: Execute the process
bool vpgl_generate_3d_point_from_depth_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_generate_3d_point_from_depth_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto u = pro.get_input<float>(i++);
  auto v = pro.get_input<float>(i++);
  auto t = pro.get_input<float>(i++);

  vgl_ray_3d<double> ray;
  if(auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr())) {
    ray=cam->backproject_ray(vgl_point_2d<double>(u,v));
  }
  else if(auto* cam = dynamic_cast<vpgl_generic_camera<double>*>(cam_ptr.ptr())) {
    ray = cam->ray(u,v);
  }
  else {
    std::cerr << "vpgl_generate_3d_point_from_depth_process: couldn't cast camera\n";
    return false;
  }
  //std::cout<<ray.origin()<<std::endl;
  vgl_point_3d<double> pt3d=ray.origin()+ray.direction()*t;

  pro.set_output_val<float>(0, (float)pt3d.x());
  pro.set_output_val<float>(1, (float)pt3d.y());
  pro.set_output_val<float>(2, (float)pt3d.z());
  return true;
}
