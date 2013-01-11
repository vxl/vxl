// This is brl/bpro/core/vpgl_pro/processes/vpgl_generate_3d_point_process_from_depth_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vsl/vsl_binary_io.h>

namespace vpgl_generate_3d_point_from_depth_process_globals
{
    const unsigned n_inputs_ = 4;
    const unsigned n_outputs_ = 3;
}
//: Init function
bool vpgl_generate_3d_point_from_depth_process_cons(bprb_func_process& pro)
{
    using namespace vpgl_generate_3d_point_from_depth_process_globals;

    //process takes 4 input
    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[0] = "vpgl_camera_double_sptr";
    input_types_[1] = "float";
    input_types_[2] = "float";
    input_types_[3] = "float";

    // process has 3 outputs
    vcl_vector<vcl_string>  output_types_(n_outputs_);
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
    vcl_cerr << "vpgl_generate_3d_point_from_depth_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  float u = pro.get_input<float>(i++);
  float v = pro.get_input<float>(i++);
  float t = pro.get_input<float>(i++);

  vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    vcl_cerr << "vpgl_generate_3d_point_from_depth_process: couldn't cast camera\n";
    return false;
  }

  vgl_ray_3d<double> ray=cam->backproject_ray(vgl_point_2d<double>(u,v));
  vcl_cout<<ray.origin()<<vcl_endl;
  vgl_point_3d<double> pt3d=ray.origin()+ray.direction()*t;
  pro.set_output_val<float>(0, (float)pt3d.x());
  pro.set_output_val<float>(1, (float)pt3d.y());
  pro.set_output_val<float>(2, (float)pt3d.z());
  return true;
}

