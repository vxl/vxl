//:
// \file
// \author Joseph Mundy
// \date  June 4, 2003

#include <testlib/testlib_test.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/algo/vgl_p_matrix.h>
static double d(double x0, double x1, double y0, double y1)
{
  return vcl_sqrt((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1));
}

static void test_identity_projection()
{
  vcl_cout << "Testing identity projection for points\n";
  vgl_homg_point_3d<double> X0(40,20,20,1), X1(10,5,5,1);
  vgl_p_matrix<double> P;
  P.set_identity();
  //The identity camera
  ///////////////////
  //      1  0  0  0
  // P =  0  1  0  0
  //      0  0  1  0
  ////////////////////
  vgl_homg_point_2d<double> hx0 = P(X0), hx1 = P(X1);
  vgl_homg_point_2d<double> hxa0 = P*X0, hxa1 = P*X1;
  vgl_point_2d<double> x0(hx0), x1(hx1);
  vgl_point_2d<double> x0a(hxa0), x1a(hxa1);
  vcl_cout << "x0 = " << x0 << '\n'
           << "x1 = " << x1 << '\n'
           << "x0a = " << x0a << '\n'
           << "x1a = " << x1a << '\n';

  double distance = d(x0.x(), x1.x(), x0.y(), x1.y());
  double distancea = d(x0.x(), x0a.x(), x0.y(), x0a.y());

  TEST_NEAR("identity", distance, 0.0, 1e-06);
  TEST_NEAR("() vs *", distancea, 0.0, 1e-06);
}

static void test_general_projection()
{
  vcl_cout << "Testing general projection for points\n";
  // Testing an actual camera projection matrix
  vnl_matrix_fixed<double, 3, 4> M;
  M.put(0,0,2.081017567642e-003);
  M.put(0,1,1.873683094206e-003);
  M.put(0,2,-3.780462177066e-004);
  M.put(0,3,3.234616994915e+000);

  M.put(1,0,2.588127173210e-004);
  M.put(1,1,-4.095391122771e-004);
  M.put(1,2,-3.152924571015e-003);
  M.put(1,3,1.239848706717e+000);

  M.put(2,0,-3.045686638048e-007);
  M.put(2,1,5.347419835901e-007);
  M.put(2,2,-3.521127567013e-007);
  M.put(2,3,3.149726840200e-003);

  vgl_p_matrix<double> P(M);

  vgl_homg_point_3d<double> X0(4.705520e+01,-7.554442e+02, 8.766468, 1);
  vgl_homg_point_3d<double> X1(3.170094e+01, -6.927508e+02, 8.766118, 1);

  vgl_homg_point_2d<double> hx0 = P(X0), hx1 = P(X1);
  vgl_point_2d<double> x0(hx0), x1(hx1);
  vcl_cout << "x0 = " << x0 << '\n'
           << "x1 = " << x1 << '\n';
// comparing with measured integer pixel coordinates
  double d0 = d(x0.x(), 701, x0.y(), 562);
  double d1 = d(x1.x(), 722 ,x1.y(), 543);
  TEST_NEAR("general", d0+d1, 0.0, 2.0);//possible 2 pixel error
}

static void test_p_matrix()
{
  test_identity_projection();
  test_general_projection();
}

TESTMAIN(test_p_matrix);
