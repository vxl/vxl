// This is brl/bpro/core/vpgl_pro/processes/vpgl_get_backproject_ray_process.cxx
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
#include <vsl/vsl_binary_io.h>
#include <vpgl/algo/vpgl_backproject.h>

namespace vpgl_get_backproject_ray_process_globals
{
    constexpr unsigned n_inputs_ = 3;
    constexpr unsigned n_outputs_ = 3;
}
//: Init function
bool vpgl_get_backproject_ray_process_cons(bprb_func_process& pro)
{
    using namespace vpgl_get_backproject_ray_process_globals;

    //process takes 4 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "vpgl_camera_double_sptr";
    input_types_[1] = "float";
    input_types_[2] = "float";


    // process has 3 outputs
    std::vector<std::string>  output_types_(n_outputs_);
    output_types_[0] = "float"; //dX
    output_types_[1] = "float"; //dY
    output_types_[2] = "float"; //dZ

    return pro.set_input_types(input_types_) &&
           pro.set_output_types(output_types_);
}

//: Execute the process
bool vpgl_get_backproject_ray_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_get_backproject_ray_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto u = pro.get_input<float>(i++);
  auto v = pro.get_input<float>(i++);


  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_get_backproject_ray_process: couldn't cast camera\n";
    return false;
  }

  vgl_ray_3d<double> ray=cam->backproject_ray(vgl_point_2d<double>(u,v));

  pro.set_output_val<float>(0, (float)ray.direction().x());
  pro.set_output_val<float>(1, (float)ray.direction().y());
  pro.set_output_val<float>(2, (float)ray.direction().z());
  return true;
}


namespace vpgl_get_rpc_backproject_ray_process_globals
{
    constexpr unsigned n_inputs_ = 7;
    constexpr unsigned n_outputs_ = 3;
}
//: Init function
bool vpgl_get_rpc_backproject_ray_process_cons(bprb_func_process& pro)
{
    using namespace vpgl_get_rpc_backproject_ray_process_globals;

    //process takes 7 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "vpgl_camera_double_sptr";
    input_types_[1] = "double";
    input_types_[2] = "double";
    input_types_[3] = "double";
    input_types_[4] = "double";
    input_types_[5] = "double";
    input_types_[6] = "double";


    // process has 3 outputs
    std::vector<std::string>  output_types_(n_outputs_);
    output_types_[0] = "double"; //dX
    output_types_[1] = "double"; //dY
    output_types_[2] = "double"; //dZ

    return pro.set_input_types(input_types_) &&
           pro.set_output_types(output_types_);
}

//: Execute the process
bool vpgl_get_rpc_backproject_ray_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_get_rpc_backproject_ray_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto u = pro.get_input<double>(i++);
  auto v = pro.get_input<double>(i++);
  auto altitude = pro.get_input<double>(i++);
  auto x0 = pro.get_input<double>(i++);
  auto y0 = pro.get_input<double>(i++);
  auto z0 = pro.get_input<double>(i++);


  auto* cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_get_rpc_backproject_ray_process: couldn't cast camera\n";
    return false;
  }

  vgl_point_3d<double> point;

  vpgl_backproject::bproj_plane(cam,
                                vgl_point_2d<double>(u,v),
                                vgl_plane_3d<double>(0, 0, 1, -altitude),
                                vgl_point_3d<double>(x0, y0, z0),
                                point,
                                0.05,
                                1.0);

////  vgl_point_3d<double> point=cam->backproject_point(vgl_point_2d<double>(u,v));

  pro.set_output_val<double>(0, (double)point.x()); // longitude
  pro.set_output_val<double>(1, (double)point.y()); // latitude
  pro.set_output_val<double>(2, (double)point.z()); // height
  return true;
}
