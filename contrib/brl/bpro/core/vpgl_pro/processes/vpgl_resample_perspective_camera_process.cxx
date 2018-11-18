// This is brl/bpro/core/vpgl_pro/processes/vpgl_resample_perspective_camera_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// this process is the companion to vil_resample_image_process and
// adjusts the calibration matrix to correspond to the resampled image
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vsl/vsl_binary_io.h>

//: Init function
bool vpgl_resample_perspective_camera_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(5);
  input_types[0] = "vpgl_camera_double_sptr";// input camera
  // the revised K matrix (no skew)
  input_types[1] = "int";// original ni
  input_types[2] = "int";// original nj
  input_types[3] = "int";// resampled ni
  input_types[4] = "int";// resampled nj
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types(1);
  output_types[0] = "vpgl_camera_double_sptr";  // resampled camera
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_resample_perspective_camera_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_resample_perspective_camera_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(0);
  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_resample_perspective_camera_process: couldn't cast camera\n";
    return false;
  }
  auto* ncam =
    new vpgl_perspective_camera<double>(*cam);
  //cast image sizes to double for scale computation
  double ni_orig = pro.get_input<int>(1), nj_orig = pro.get_input<int>(2);
  double ni_new = pro.get_input<int>(3), nj_new = pro.get_input<int>(4);
  if (ni_orig == 0.0||nj_orig == 0.0||ni_new == 0.0||nj_new == 0.0){
    std::cout << "In vpgl_resample_perspective_camera_process -"
             << " zero image dimension(s)\n";
    return false;
  }
  //the origin of the two images is the same, so the transformation from
  //orig -> new is just a scale transformation
  //
  //     [ni_new/ni_orig     0          0]
  // T = [     0       nj_new/nj_orig   0]
  //     [     0             0          1]
  //
  double su = ni_new/ni_orig, sv = nj_new/nj_orig;
  vnl_matrix_fixed<double, 3, 3> T(0.0);
  T[0][0] = su;   T[1][1] = sv; T[2][2] = 1.0;
  vnl_matrix_fixed<double,3,3> M = cam->get_calibration().get_matrix();
  M = T*M;
  vpgl_calibration_matrix<double> K(M);
  ncam->set_calibration(K);

  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}
