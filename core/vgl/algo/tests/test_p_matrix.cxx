//:
// \file
// \author Joseph Mundy
// \date  June 4, 2003
//
// \verbatim
//  Modifications
//   2009-03-08 Peter Vanroose - Increased the test coverage by adding tests for
//                               constructors, read/write, and accessors.
// \endverbatim

#include <cmath>
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_p_matrix.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_matrix.h>

static double d(double x0, double x1, double y0, double y1)
{
  return std::sqrt((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1));
}

static bool equals(const double x[12], const double y[12])
{
  for (int i=0; i<12; ++i) if (x[i] != y[i]) return false;
  return true;
}

static void test_constructors()
{
  double data[12]; // the projective c-matrix
  {
    double gold[] = {1,0,0,0, 0,1,0,0, 0,0,1,0}; // the "ground truth"
    vgl_p_matrix<double> P; P.get(data);
    TEST( "Default constructor", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vgl_p_matrix<double> P0(gold);
    const vgl_p_matrix<double>& P(P0); P.get(data);
    TEST( "Copy constructor", equals(data, gold), true );
  }
  {
    std::stringstream ss; ss << "1 2 3 4 5 6 7 8 9 10 11 12";
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vgl_p_matrix<double> P(ss); P.get(data);
    TEST( "Constructor from istream", equals(data, gold), true );
  }
  {
    std::stringstream ss; ss << "1 2 3 4 5 6 7 8 9 10 11 12";
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vgl_p_matrix<double> P; ss >> P; P.get(data);
    TEST( "operator>> from istream", equals(data, gold), true );
  }
  {
    std::stringstream ss; ss << "1 2 3 4 5 6 7 8 9 10 11 12";
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vgl_p_matrix<double> P; P.read_ascii(ss); P.get(data);
    TEST( "read_ascii() method", equals(data, gold), true );
  }
  {
    std::stringstream ss; ss << "1 2 3 4 5 6 7 8 9 10 11 12";
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vgl_p_matrix<double> P = vgl_p_matrix<double>::read(ss); P.get(data);
    TEST( "read() static method", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vnl_double_3x4 M(gold);
    vgl_p_matrix<double> P(M); P.get(data);
    TEST( "Constructor from vnl_double_3x4", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vgl_p_matrix<double> P(gold); P.get(data);
    TEST( "Constructor from raw 3x4 data array", equals(data, gold), true );
  }
  {
    double gold[] = {7,7,7,30, 7,7,7,40, 7,7,7,50}; // the "ground truth"
    vnl_double_3x3 M(7.0);     vnl_matrix<double> m = M.as_ref();
    vnl_double_3 V(30,40,50);  vnl_vector<double> v = V.as_ref();
    vgl_p_matrix<double> P(M,V); P.get(data);
    TEST( "Constructor from vnl_double_3x3 and vnl_double_3", equals(data, gold), true );
    P.get(&m,&v);
    vgl_p_matrix<double> P2(m,v); P2.get(data);
    TEST( "get(vnl_matrix*,vnl_vector*)", equals(data, gold), true );
    P.get(&M,&V);
    vgl_p_matrix<double> P3(M,V); P3.get(data);
    TEST( "get(vnl_double_3x3*,vnl_double_3*)", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vgl_p_matrix<double> P0(gold);
    vgl_p_matrix<double> P; P = P0; P.get(data);
    TEST( "Assignment operator", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vgl_p_matrix<double> P; P.set(gold); P.get(data);
    TEST( "set(array)", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    double matrix[][4] = {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}}; // the "ground truth"
    vgl_p_matrix<double> P; P.set(matrix); P.get(data);
    TEST( "set(2D array)", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vnl_double_4 v1(1,2,3,4), v2(5,6,7,8), v3(9,10,11,12);
    vgl_p_matrix<double> P; P.set_rows(v1,v2,v3); P.get(data);
    TEST( "set_rows()", equals(data, gold), true );
  }
  {
    double gold[] = {1,2,3,4, 5,6,7,8, 9,10,11,12}; // the "ground truth"
    vnl_double_3x4 M(gold);
    vgl_p_matrix<double> P; P.set(M); P.get(data);
    TEST( "set(vnl_double_3x4)", equals(data, gold), true );
    vnl_matrix<double> m = M.as_ref(); P.set(m); P.get(data);
    TEST( "set(vnl_matrix)", equals(data, gold), true );
  }
  {
    double gold[] = {7,7,7,30, 7,7,7,40, 7,7,7,50}; // the "ground truth"
    vnl_double_3x3 M(7.0);
    vnl_double_3 V(30,40,50);
    vgl_p_matrix<double> P; P.set(M,V); P.get(data);
    TEST( "set(matrix,vector)", equals(data, gold), true );
  }
}

static void test_accessors()
{
  double rotation[] = {1,0,0,0, 0,0.6,0.8,0, 0,-0.8,0.6,0, 0,0,0,1}; // Euclidean rotation around (0,0,0,1)
  vgl_p_matrix<double> P(rotation);
  vnl_double_3x4 M(rotation), M1;
  vnl_matrix<double> m(rotation, 3,4), M2;

  TEST( "get(row,col)", P.get(1,2), 0.8 );
  TEST( "get_matrix()", P.get_matrix(), M );
  P.get(M1); TEST( "get(vnl_double_3x4)", M1, M );
  P.get(M2); TEST( "get(vnl_matrix)", M2, m );
  vnl_vector<double> v1, v2, v3; P.get_rows(&v1,&v2,&v3);
  TEST( "get_rows(vnl_vector 3x)", v1==vnl_double_4(1,0,0,0) && v2==vnl_double_4(0,0.6,0.8,0) && v3==vnl_double_4(0,-0.8,0.6,0), true );
  vnl_double_4 d1, d2, d3; P.get_rows(&d1,&d2,&d3);
  TEST( "get_rows(vnl_double_4 3x)", d1==v1 && d2==v2 && d3==v3, true );
  std::stringstream ss; ss << P;
  TEST( "operator<< to ostream", P, vgl_p_matrix<double>(ss) );
  TEST_NEAR( "operator()(point projection)", vgl_distance(P(vgl_homg_point_3d<double>(1.0,2.0,0.0,0.0)),vgl_homg_point_2d<double>(1.0,1.2,-1.6)), 0, 1e-10 );
  TEST( "get_focal()", P.get_focal(), vgl_homg_point_3d<double>(0.0,0.0,0.0,1.0) );
  rotation[6] *= -1; rotation[9] *= -1; vgl_h_matrix_3d<double> H(rotation);
  TEST( "get_canonical_H()", P.get_canonical_H(), H );
  TEST( "is_canonical() (small tolerance)", P.is_canonical(0.75), false );
  TEST( "is_canonical() (larger tolerance)", P.is_canonical(0.8), true );
}

static void test_cheirality()
{
  double rotation[] = {1,0,0,0, 0,-0.6,0.8,0, 0,0.8,0.6,0}; // Euclidean rotation around (0,0,0,1)
  vgl_p_matrix<double> P(rotation);
  P.fix_cheirality(); // this should flip P's sign since rotation[] has det = -1
  P.flip_sign();      // flip back
  TEST( "fix_cheirality()", P.get_matrix(), vnl_double_3x4(rotation) );
}

static void test_homog_product()
{
  vgl_p_matrix<double>    P; P.set_identity();
  vgl_h_matrix_3d<double> H; H.set_identity();

  vgl_p_matrix<double> P2 = P * H;

  TEST( "Projection * homography", P2.get_matrix(), P.get_matrix() );
}

static void test_identity_projection()
{
  std::cout << "Testing identity projection for points\n";
  vgl_homg_point_3d<double> X0(40,20,20,1), X1(10,5,5,1);
  vgl_p_matrix<double> P; P.set_identity();
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
  std::cout << "x0 = " << x0 << '\n'
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
  std::cout << "Testing general projection for points\n";
  // Testing an actual camera projection matrix
  vnl_double_3x4 M;
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
  std::cout << "x0 = " << x0 << '\n'
           << "x1 = " << x1 << '\n';
// comparing with measured integer pixel coordinates
  double d0 = d(x0.x(), 701, x0.y(), 562);
  double d1 = d(x1.x(), 722 ,x1.y(), 543);
  TEST_NEAR("general", d0+d1, 0.0, 2.0);//possible 2 pixel error
}

static void test_p_matrix()
{
  std::cout << "\n==================== test_constructors ====================\n\n";
  test_constructors();
  std::cout << "\n===================== test_accessors ======================\n\n";
  test_accessors();
  std::cout << "\n================ test_identity_projection =================\n\n";
  test_identity_projection();
  std::cout << "\n================= test_general_projection =================\n\n";
  test_general_projection();
  std::cout << "\n=================== test_homog_product ====================\n\n";
  test_homog_product();
  std::cout << "\n===================== test_cheirality =====================\n\n";
  test_cheirality();
}

TESTMAIN(test_p_matrix);
