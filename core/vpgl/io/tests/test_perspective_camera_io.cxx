#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <vnl/vnl_double_3.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/io/vgl_io_point_3d.h>
#include <vnl/vnl_matrix_fixed.h>
static double camera_diff_norm(vpgl_perspective_camera<double> const& C0,
                               vpgl_perspective_camera<double> const& C1)
{
  vnl_matrix_fixed<double,3,4> dif = C0.get_matrix()- C1.get_matrix();
  return dif.fro_norm();
}
static void test_perspective_camera_io()
{
  std::cout << "Testing perspective camera" << std::endl;

  // ===========  Construct the camera

  double data[] = { 2000,    0, 512,
                    0, 2000, 384,
                    0,    0,   1 };
  vpgl_calibration_matrix<double> K = vnl_double_3x3(data);
  vgl_homg_point_3d<double>center(0,0,-10.0);

  // rotation angle in radians
  double theta = vnl_math::pi_over_4; // 45 degrees
  // y axis is the rotation axis
  vnl_double_3 axis(0.0, 1.0, 0.0);
  vgl_h_matrix_3d<double> R;
  R.set_identity().set_rotation_about_axis(axis, theta);
  std::cout <<"Rotation Matrix\n" << R << '\n';
  vpgl_perspective_camera<double> P(K, center, vgl_rotation_3d<double>(R));

  std::cout << "Camera " << P;

  // test output

  vsl_b_ofstream bp_out("test_perspective_camera_io.tmp");
  TEST("Created test_perspective_camera_io.tmp for writing",(!bp_out), false);
  vsl_b_write(bp_out, P);
  bp_out.close();

  // test input binary stream

  vsl_b_ifstream bp_in("test_perspective_camera_io.tmp");
  TEST("Opened test_perspective_camera_io.tmp for reading",(!bp_in), false);

  vpgl_perspective_camera<double> P_r;
  vsl_b_read(bp_in, P_r);
  bp_in.close();
  vpl_unlink("test_perspective_camera_io.tmp");
  std::cout << "Recovered Camera " << P_r;
  double er = camera_diff_norm(P, P_r);
  TEST_NEAR("recovered camera", er, 0.0, 1e-3);
}

TESTMAIN(test_perspective_camera_io);
