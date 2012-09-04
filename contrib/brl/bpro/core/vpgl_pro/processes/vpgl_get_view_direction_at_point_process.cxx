// This is brl/bpro/core/vpgl_pro/processes/vpgl_get_view_direction_at_point_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vsl/vsl_binary_io.h>

namespace vpgl_get_view_direction_at_point_process_globals
{
    const unsigned n_inputs_ = 4;
    const unsigned n_outputs_ = 2;
}
//: Init function
bool vpgl_get_view_direction_at_point_process_cons(bprb_func_process& pro)
{
    using namespace vpgl_get_view_direction_at_point_process_globals;

    //process takes 4 inputs
    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[0] = "vpgl_camera_double_sptr";
    input_types_[1] = "float";
    input_types_[2] = "float";
    input_types_[3] = "float";

    // process has 2 outputs
    vcl_vector<vcl_string>  output_types_(n_outputs_);
    output_types_[0] = "float"; // theta
    output_types_[1] = "float"; // phi

    bool good = pro.set_input_types(input_types_) &&
                pro.set_output_types(output_types_);
    return good;
}

//: Execute the process
bool vpgl_get_view_direction_at_point_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    vcl_cerr << "vpgl_get_view_direction_at_point_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  float x = pro.get_input<float>(i++);
  float y = pro.get_input<float>(i++);
  float z = pro.get_input<float>(i++);

  vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    vcl_cerr << "vpgl_get_view_direction_at_point_process: couldn't cast camera\n";
    return false;
  }

  vgl_point_3d<double> camcenter = cam->camera_center();
  vgl_vector_3d<double> view_direction = vgl_point_3d<double>(x,y,z)-camcenter;
  normalize(view_direction);

  float theta = (float)vcl_acos(view_direction.z());
  float phi   = (float)vcl_atan2(view_direction.y(),view_direction.x());
  pro.set_output_val<float>(0, theta);
  pro.set_output_val<float>(1, phi);
  return true;
}

