// This is brl/bpro/core/vpgl_pro/processes/vpgl_get_backproject_ray_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vsl/vsl_binary_io.h>

namespace vpgl_get_backproject_ray_processs_globals
{
    const unsigned n_inputs_ = 3;
    const unsigned n_outputs_ = 3;
}
//: Init function
bool vpgl_get_backproject_ray_processs_cons(bprb_func_process& pro)
{
    using namespace vpgl_get_backproject_ray_processs_globals;

    //process takes 4 input
    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[0] = "vpgl_camera_double_sptr";
    input_types_[1] = "float";
    input_types_[2] = "float";


    // process has 3 outputs
    vcl_vector<vcl_string>  output_types_(n_outputs_);
    output_types_[0] = "float"; //dX
    output_types_[1] = "float"; //dY
    output_types_[2] = "float"; //dZ

    return pro.set_input_types(input_types_) &&
           pro.set_output_types(output_types_);
}

//: Execute the process
bool vpgl_get_backproject_ray_processs(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    vcl_cerr << "vpgl_get_backproject_ray_processs: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  float u = pro.get_input<float>(i++);
  float v = pro.get_input<float>(i++);
  

  vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    vcl_cerr << "vpgl_get_backproject_ray_processs: couldn't cast camera\n";
    return false;
  }

  vgl_ray_3d<double> ray=cam->backproject_ray(vgl_point_2d<double>(u,v));

  pro.set_output_val<float>(0, (float)ray.direction().x());
  pro.set_output_val<float>(1, (float)ray.direction().y());
  pro.set_output_val<float>(2, (float)ray.direction().z());
  return true;
}

