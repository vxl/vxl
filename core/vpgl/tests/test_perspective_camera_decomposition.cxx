#include <testlib/testlib_test.h>
#include <vpgl/vpgl_perspective_camera.h>

static void test_perspective_camera_decomposition()
{
  vnl_matrix_fixed<double,3,4> P;
  P[0][0] = 3.53553e2;
  P[0][1] = 3.39645e2;
  P[0][2] = 2.77744e2;
  P[0][3] = -1.44946e6;

  P[1][0] = -1.03528e2;
  P[1][1] = 2.33212e1;
  P[1][2] = 4.59607e2;
  P[1][3] = -6.32525e5;

  P[2][0] = 7.07107e-1;
  P[2][1] = -3.53553e-1;
  P[2][2] = 6.12372e-1;
  P[2][3] = -9.18559e2;


  vpgl_perspective_camera<double> camera;
  bool did_decompose = vpgl_perspective_decomposition(P, camera);
  bool is_correct = false;

  if ( did_decompose )
  {
    vnl_matrix_fixed<double,3,3> K;
    K[0][0] = 468.2;    K[0][1] = 91.2;     K[0][2] = 300;
    K[1][0] = 0;        K[1][1] = 427.2;    K[1][2] = 200;
    K[2][0] = 0;        K[2][1] = 0;        K[2][2] = 1;

    vnl_matrix<double> R(3,3);
    R[0][0] = 0.41380;  R[0][1] = 0.90915;  R[0][2] = 0.04708;
    R[1][0] = -0.57338; R[1][1] = 0.22011;  R[1][2] = 0.78917;
    R[2][0] = 0.70711;  R[2][1] = -0.35355; R[2][2] = 0.61237;

    is_correct = std::abs((K-camera.get_calibration().get_matrix()).frobenius_norm()) < 0.1 &&
                 std::abs((R-camera.get_rotation().as_matrix()).frobenius_norm() ) < 0.0001;
  }
  TEST( "Hartley & Zisserman p. 163", did_decompose && is_correct, true);
}

TESTMAIN(test_perspective_camera_decomposition);
