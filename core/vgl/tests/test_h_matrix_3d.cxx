//:
// \file
// \author Joseph Mundy
// \date  March 28, 2003
//
// \verbatim
//  Modifications
//   2009-03-08 Peter Vanroose - Increased the test coverage by adding tests for
//                               all basic constructors (not for point list match).
// \endverbatim

#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4x4.h>

static bool equals(double x[16], double y[16])
{
  for (int i=0; i<12; ++i) if (x[i] != y[i]) return false;
  return true;
}

static void test_constructors()
{
  double data[16]; // the projective h-matrix
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}; // the "ground truth"
    vgl_h_matrix_3d<double> H(gold); H.get(data);
    TEST( "Constructor from raw 4x4 data array", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}; // the "ground truth"
    vgl_h_matrix_3d<double> H0(gold);
    vgl_h_matrix_3d<double> H(H0); H.get(data);
    TEST( "Copy constructor", equals(data, gold), true );
  }
  {
    vcl_stringstream ss; ss << "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16";
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}; // the "ground truth"
    vgl_h_matrix_3d<double> H(ss); H.get(data);
    TEST( "Constructor from istream", equals(data, gold), true );
  }
  {
    vcl_stringstream ss; ss << "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16";
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}; // the "ground truth"
    vgl_h_matrix_3d<double> H; ss >> H; H.get(data);
    TEST( "operator>> from istream", equals(data, gold), true );
  }
  {
    vcl_stringstream ss; ss << "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16";
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}; // the "ground truth"
    vgl_h_matrix_3d<double> H; H.read(ss); H.get(data);
    TEST( "read() method", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}; // the "ground truth"
    vnl_double_4x4 M(gold);
    vgl_h_matrix_3d<double> H(M); H.get(data);
    TEST( "Constructor from vnl_double_4x4", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}; // the "ground truth"
    vgl_h_matrix_3d<double> H0(gold);
    vgl_h_matrix_3d<double> H; H = H0; H.get(data);
    TEST( "Assignment operator", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}; // the "ground truth"
    vgl_h_matrix_3d<double> H; H.set(gold); H.get(data);
    TEST( "set(array)", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}; // the "ground truth"
    vnl_double_4x4 M(gold);
    vgl_h_matrix_3d<double> H; H.set(M); H.get(data);
    TEST( "set(matrix)", equals(data, gold), true );
  }
}

static void test_identity_transform()
{
  vcl_cout << "Testing identity transform on point\n";
  vnl_double_4x4 M;
  M.set_identity();
  vgl_h_matrix_3d<double> Id(M);
  vgl_homg_point_3d<double> p(4,3,2,1), pp;
  pp = Id(p);
  vcl_cout << "Id = " << Id << '\n'
           << "p = " << p << " , Id(p) = " << pp << '\n';
  vgl_point_3d<double> xp(p), xpp(pp);
  double distance = vcl_sqrt((xp.x()-xpp.x())*(xp.x()-xpp.x()) +
                             (xp.y()-xpp.y())*(xp.y()-xpp.y()) +
                             (xp.z()-xpp.z())*(xp.z()-xpp.z()));
  TEST_NEAR("identity", distance, 0.0, 1e-06);
}

static void test_perspective_transform()
{
  vcl_cout << "Testing perspective transform on point\n";
  vnl_double_4x4 M;
  vgl_homg_point_3d<double> p(3,2,1), pp, ppp;
  M.put(0,0,1);    M.put(0,1,2);  M.put(0,2,1),    M.put(0,3,1.25);
  M.put(1,0,0.5);  M.put(1,1,-2); M.put(1,2,1.5),  M.put(1,3,2.25);
  M.put(2,0,0.25); M.put(2,1,3);  M.put(2,2,1.75), M.put(2,3,5.1);
  M.put(3,0,0.15); M.put(3,1,4);  M.put(3,2,8.5),  M.put(3,3,10);
  vgl_h_matrix_3d<double> Tproj(M);
  pp = Tproj(p);
  ppp = Tproj.preimage(pp);
  vcl_cout << "Tproj\n" << Tproj << '\n'
           << "p = " << p << " , Tproj(p) = pp = " << pp << '\n'
           << " , Tproj.preimage(pp) = " << ppp << '\n';
  vgl_point_3d<double> xp(p), xppp(ppp);
  double distance = vcl_sqrt((xp.x()-xppp.x())*(xp.x()-xppp.x()) +
                             (xp.y()-xppp.y())*(xp.y()-xppp.y()) +
                             (xp.z()-xppp.z())*(xp.z()-xppp.z()));
  TEST_NEAR("perspective",distance , 0.0, 1e-06);
}

static void test_projective_basis()
{
  //
  //projective basis 3d not yet implemented
  //
}

static void test_rotation_about_axis()
{
  vgl_h_matrix_3d<double> R;
  R.set_identity();
  vnl_double_3 v(0,0,1.0);
  R.set_rotation_about_axis(v, .785398);//rotate 45 degrees
  vcl_cout << "Rotation Matrix\n" << R << '\n';
  vgl_homg_point_3d<double> p(1,0,0,1), pp; //point on x axis
  pp = R(p);
  vcl_cout << "p = " << p << " , R(p) = " << pp << '\n';
  vgl_point_3d<double> xpp(pp);
  double distance = vcl_sqrt((xpp.x()-0.707)*(xpp.x()-0.707) +
                             (xpp.y()-0.707)*(xpp.y()-0.707) +
                             xpp.z()*xpp.z());
  TEST_NEAR("rotation",distance , 0.0, 1e-03);
}

static void test_h_matrix_3d()
{
  vcl_cout << "\n==================== test_constructors ====================\n\n";
  test_constructors();
  vcl_cout << "\n================= test_identity_transform =================\n\n";
  test_identity_transform();
  vcl_cout << "\n=============== test_perspective_transform ================\n\n";
  test_perspective_transform();
  vcl_cout << "\n================== test_projective_basis ==================\n\n";
  test_projective_basis();
  vcl_cout << "\n================ test_rotation_about_axis =================\n\n";
  test_rotation_about_axis();
}

TESTMAIN(test_h_matrix_3d);
