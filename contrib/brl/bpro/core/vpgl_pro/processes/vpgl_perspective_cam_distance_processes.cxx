// This is brl/bpro/core/vpgl_pro/processes/vpgl_perspective_cam_distance_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

// compute the distance between two distance cameras as the angle between their principle axis vectors
namespace vpgl_persp_cam_distance_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}

//: Init function
bool vpgl_persp_cam_distance_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_persp_cam_distance_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "float"; // distance

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);
  return good;
}

//: Execute the process
bool vpgl_persp_cam_distance_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    vcl_cerr << "vpgl_persp_cam_distance_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam1_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam2_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);

  vpgl_perspective_camera<double>* cam1 = dynamic_cast<vpgl_perspective_camera<double>*>(cam1_ptr.ptr());
  if (!cam1) {
    vcl_cerr << "vpgl_persp_cam_distance_process: couldn't cast camera\n";
    return false;
  }
  vpgl_perspective_camera<double>* cam2 = dynamic_cast<vpgl_perspective_camera<double>*>(cam2_ptr.ptr());
  if (!cam2) {
    vcl_cerr << "vpgl_persp_cam_distance_process: couldn't cast camera\n";
    return false;
  }
  double dist = vpgl_persp_cam_distance(*cam1, *cam2);
  pro.set_output_val<float>(0, (float)dist);
  return true;
}

