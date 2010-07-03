// This is brl/bpro/core/vpgl_pro/processes/vpgl_create_perspective_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vsl/vsl_binary_io.h>

//: Init function
bool vpgl_create_perspective_camera_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types(10);
  // the revised K matrix (no skew)
  input_types[0] = "double";// K[0][0] - u scale
  input_types[1] = "double";// K[0][2] - principal point (u)
  input_types[2] = "double";// K[1][1] - v scale
  input_types[3] = "double";// K[1][2] - principal point (v)
  input_types[4] = "double";// center x
  input_types[5] = "double";// center y
  input_types[6] = "double";// center z
  input_types[7] = "double";// look at point x
  input_types[8] = "double";// look at point y
  input_types[9] = "double";// look at point z
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types(1);
  output_types[0] = "vpgl_camera_double_sptr";  // output camera
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_create_perspective_camera_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    vcl_cerr << "vpgl_scale_perspective_camera_process: Invalid inputs\n";
    return false;
  }
  // get the inputs

  double k00 = pro.get_input<double>(0), k02 = pro.get_input<double>(1);
  double k11 = pro.get_input<double>(2), k12 = pro.get_input<double>(3);
  vnl_matrix_fixed<double ,3,3> m(0.0);
  m[0][0]=k00;   m[0][2]=k02;   m[1][1]=k11;   m[1][2]=k12; m[2][2]=1.0;
  vpgl_calibration_matrix<double> K(m);

  vgl_homg_point_3d<double> cent(pro.get_input<double>(4), pro.get_input<double>(5), pro.get_input<double>(6));
  vgl_rotation_3d<double> I; // no rotation initially
  vpgl_perspective_camera<double> camera(K, cent, I);

  vgl_homg_point_3d<double> look(pro.get_input<double>(7), pro.get_input<double>(8), pro.get_input<double>(9));
  camera.look_at(look);

  vpgl_perspective_camera<double>* ncam = new vpgl_perspective_camera<double>(camera);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}

