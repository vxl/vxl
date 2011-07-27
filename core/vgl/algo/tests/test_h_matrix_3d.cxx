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
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_math.h>
#include <vgl/algo/vgl_h_matrix_3d_compute_linear.h>
#include <vnl/vnl_det.h>

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
  vgl_h_matrix_3d<double> Id(vnl_double_4x4().set_identity());
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
  vgl_h_matrix_3d<double> R; R.set_identity();
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

static void test_compute_linear_points()
{
  vcl_cout << "\n=== Test the recovery of a general homography using the linear algorithm ===\n";
  vcl_vector<vgl_homg_point_3d<double> > points1, points2;

  //setup the first set of points,  no 4 of them should be co-planar
  vgl_homg_point_3d<double> p10(100.0, 50.0, 100.0), p11(100.0, 50.0, 200.0);
  vgl_homg_point_3d<double> p12(200.0, 50.0, 200.0), p13(100.0, 200.0, 200.0);

  vgl_homg_point_3d<double> p14(300.0, 25.0, 0.0), p15(350.0, 25.0, 0.0);
  vgl_homg_point_3d<double> p16(300.0, 25.0, 250.0), p17(280.0, 100.0, 250.0);

  vgl_homg_point_3d<double> p18(250.0, 75.0, 300.0), p19(250.0, 75.0, 0.0);


  points1.push_back(p10); points1.push_back(p11); points1.push_back(p12);
  points1.push_back(p13); points1.push_back(p14); points1.push_back(p15);
  points1.push_back(p16); points1.push_back(p17); points1.push_back(p18);
  points1.push_back(p19);

  //: setup an initial homography
  vgl_h_matrix_3d<double> H1; H1.set_identity().set_rotation_roll_pitch_yaw(45.0*vnl_math::pi_over_180, 15.0*vnl_math::pi_over_180, 10.0*vnl_math::pi_over_180);
  vgl_h_matrix_3d<double> H2; H2.set_identity().set_translation(5.0, 50.0, 150.0);
  vgl_h_matrix_3d<double> gt_H = H1*H2;

  vcl_cout << "The gt transform\n" << gt_H << '\n';

  //: transform the points
  for (unsigned i = 0; i < points1.size(); i++)
    points2.push_back(gt_H(points1[i]));

  vgl_h_matrix_3d_compute_linear hmcl;
  vgl_h_matrix_3d<double> H = hmcl.compute(points1, points2);

  vcl_cout << "The resulting transform\n" << H << '\n';

  vgl_homg_point_3d<double> p_test_hom(150.0, 75.0, 100.0);
  vgl_point_3d<double> p_test(p_test_hom);
  vgl_point_3d<double> p_test_mapped(gt_H(p_test_hom));
  vgl_point_3d<double> p_test_mapped2(H(p_test_hom));
  vcl_cout << "supposed to map: " << p_test << " to " << p_test_mapped << '\n'
           << "maps: " << p_test_mapped2 << vcl_endl;

  double dist = vgl_distance(p_test_mapped, p_test_mapped2);
  vcl_cout << " dist: " << dist << vcl_endl;
  TEST_NEAR("testing computed H", dist, 0.0, 5e-03);

  //: setup a general homography
  vnl_matrix_fixed<double, 4, 4> H_m;
  H_m(0,0)=2.0; H_m(0,1)=1.5; H_m(0,2)=3.0; H_m(0,3)=4.0;
  H_m(1,0)=3.0; H_m(1,1)=3.5; H_m(1,2)=4.0; H_m(1,3)=4.5;
  H_m(2,0)=2.5; H_m(2,1)=1.5; H_m(2,2)=1.0; H_m(2,3)=5.0;
  H_m(3,0)=5.5; H_m(3,1)=6.5; H_m(3,2)=1.0; H_m(3,3)=2.5;
  vgl_h_matrix_3d<double> gt_H2(H_m);

  vcl_cout << "The gt transform\n" << gt_H2 << '\n';

  points2.clear();
  //: transform the points
  for (unsigned i = 0; i < points1.size(); i++)
    points2.push_back(gt_H2(points1[i]));

  vgl_h_matrix_3d_compute_linear hmcl2;
  vgl_h_matrix_3d<double> H2o = hmcl2.compute(points1, points2);

  vcl_cout << "The resulting transform\n" << H2o << '\n';

  p_test_mapped = gt_H2(p_test_hom);
  p_test_mapped2 = H2o(p_test_hom);
  vcl_cout << "supposed to map: " << p_test << " to " << p_test_mapped << '\n'
           << "maps: " << p_test_mapped2 << vcl_endl;

  dist = vgl_distance(p_test_mapped, p_test_mapped2);
  vcl_cout << " dist: " << dist << vcl_endl;
  TEST_NEAR("testing computed H2o", dist, 0.0, 5e-03);
}

static void test_reflection_about_plane()
{
  vgl_h_matrix_3d<double> H;
  vgl_plane_3d<double> plane(1,2,3,4);
  H.set_reflection_plane(plane);
  TEST_NEAR("determinant(reflection)", vnl_det(H.get_matrix()), -1, 1e-8);

  vgl_point_3d<double> p1(10,10,10), p2(-20, -30, 50);
  vgl_point_3d<double> p1r = H*vgl_homg_point_3d<double>(p1);
  vgl_point_3d<double> p2r = H*vgl_homg_point_3d<double>(p2);
  double plane_dist1 = vgl_distance(plane,p1);
  double reflect_dist1 = vgl_distance(p1,p1r);
  double plane_dist2 = vgl_distance(plane,p2);
  double reflect_dist2 = vgl_distance(p2,p2r);
  TEST_NEAR("reflection distance",
            vcl_max(vcl_abs(plane_dist1-reflect_dist1/2.0),
                    vcl_abs(plane_dist2-reflect_dist2/2.0)), 0.0, 1e-8);

  double plane_err1 = vgl_distance(vgl_closest_point(plane,p1),midpoint(p1,p1r));
  double plane_err2 = vgl_distance(vgl_closest_point(plane,p2),midpoint(p2,p2r));
  TEST_NEAR("reflection midpoint",
            vcl_max(plane_err1, plane_err2), 0.0, 1e-8);

  vgl_point_3d<double> p1rr = H*vgl_homg_point_3d<double>(p1r);
  vgl_point_3d<double> p2rr = H*vgl_homg_point_3d<double>(p2r);
  TEST_NEAR("reflection reversible",
            vcl_max(vgl_distance(p1,p1rr),vgl_distance(p2,p2rr)),
            0.0, 1e-8);
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
  vcl_cout << "\n=============== test_compute_linear_points ================\n\n";
  test_compute_linear_points();
  vcl_cout << "\n=============== test_reflection_about_plane ===============\n\n";
  test_reflection_about_plane();
}

TESTMAIN(test_h_matrix_3d);
