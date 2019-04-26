#include <testlib/testlib_test.h>

#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>
#include <vnl/vnl_math.h>

static void test_affine_fundamental_matrix_double()
{
  vnl_matrix_fixed<double, 3,4> M(0.0), Mp(0.0);
  M[2][3] = 1.0;   Mp[2][3] = 1.0; // affine
  M[0][0] = 1.0;   M[1][1] = 1.0; M[1][3] = 10.0;
  double s2 = sqrt(2.0)/2.0;
  Mp[0][0] = s2; Mp[0][2] = s2; Mp[0][3] = -3.12132; Mp[1][1] = 1.0;
  vpgl_affine_camera<double> cr(M), cl(Mp);
  vpgl_affine_fundamental_matrix<double> F(cr, cl);
  vnl_matrix_fixed<double, 3, 3> Mf = F.get_matrix();
  double er = fabs(Mf[1][2] + 1) + fabs(Mf[2][1] - 1) + fabs(Mf[2][2] + 10.0);
  bool good =  er < 0.001;
  TEST("compute F matrix (double)", good, true);
}

static void test_affine_fundamental_matrix_float()
{
  vnl_matrix_fixed<float, 3,4> M(0.0), Mp(0.0);
  M[2][3] = 1.0f;   Mp[2][3] = 1.0f; // affine
  M[0][0] = 1.0f;   M[1][1] = 1.0f; M[1][3] = 10.0f;
  float s2 = sqrt(2.0)/2.0;
  Mp[0][0] = s2; Mp[0][2] = s2; Mp[0][3] = -3.12132f; Mp[1][1] = 1.0f;
  vpgl_affine_camera<float> cr(M), cl(Mp);
  vpgl_affine_fundamental_matrix<float> F(cr, cl);
  vnl_matrix_fixed<float, 3, 3> Mf = F.get_matrix();
  double er = fabs(Mf[1][2] + 1) + fabs(Mf[2][1] - 1) + fabs(Mf[2][2] + 10.0);
  bool good =  er < 0.001;
  TEST("compute F matrix (float)", good, true);
}

static void test_affine_fundamental_matrix()
{
  test_affine_fundamental_matrix_float();
  test_affine_fundamental_matrix_double();
}

TESTMAIN(test_affine_fundamental_matrix);
