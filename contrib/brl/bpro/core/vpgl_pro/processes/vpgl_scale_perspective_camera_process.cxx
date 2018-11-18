// This is brl/bpro/core/vpgl_pro/processes/vpgl_scale_perspective_camera_process.cxx
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
#include <vpgl/vpgl_calibration_matrix.h>
#include <vsl/vsl_binary_io.h>

//: Init function
bool vpgl_scale_perspective_camera_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(5);
  input_types[0] = "vpgl_camera_double_sptr";// input camera
  // the revised K matrix (no skew)
  input_types[1] = "double";// K[0][0] - u scale
  input_types[2] = "double";// K[0][2] - principal point (u)
  input_types[3] = "double";// K[1][1] - v scale
  input_types[4] = "double";// K[1][2] - principal point (v)
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types(1);
  output_types[0] = "vpgl_camera_double_sptr";  // scaled camera
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_scale_perspective_camera_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_scale_perspective_camera_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(0);
  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_scale_perspective_camera_process: couldn't cast camera\n";
    return false;
  }
  auto* ncam =
    new vpgl_perspective_camera<double>(*cam);
  auto k00 = pro.get_input<double>(1), k02 = pro.get_input<double>(2);
  auto k11 = pro.get_input<double>(3), k12 = pro.get_input<double>(4);
  vnl_matrix_fixed<double ,3,3> m(0.0);
  m[0][0]=k00;   m[0][2]=k02;   m[1][1]=k11;   m[1][2]=k12; m[2][2]=1.0;
  vpgl_calibration_matrix<double> K(m);
  ncam->set_calibration(K);

  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}
