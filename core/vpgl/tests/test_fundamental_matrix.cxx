#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_fundamental_matrix.h>

#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/vpgl_essential_matrix.h>

static vnl_double_3x3
skew_symmetric(const double tx, const double ty, const double tz)
{
  vnl_double_3x3 m;
  m[0][0] =  0;   m[0][1] = -tz;  m[0][2] =  ty;
  m[1][0] =  tz;  m[1][1] =  0;   m[1][2] = -tx;
  m[2][0] = -ty;  m[2][1] =  tx;  m[2][2] =  0;
  return m;
}

static vpgl_fundamental_matrix<double> actual_f_matrix()
{
  vnl_double_3x3 m;

  m[0][0] = 0;  m[0][1] = -9.46971705e-006; m[0][2] = 2.31061096e-003;
  m[1][0] = 0;  m[1][1] = 0;                m[1][2] = 1.66666667e-002;
  m[2][0] = 0;  m[2][1] = -6.81088136e-003; m[2][2] = -2.33814495e+000;

  return vpgl_fundamental_matrix<double>(m);
}

static void test_fundamental_matrix()
{
  // Test constructor from cameras:

  double random_list1r[12] = { 1, 15, 9, -1, 2, -6, -9, 7, -5, 6, 10, 0 };
  double random_list1l[12] = { 10.6, 1.009, .676, .5, -13, -10, 8, 5, 88, -2, -100, 11 };
  vpgl_proj_camera<double> C1r( random_list1r );
  vpgl_proj_camera<double> C1l( random_list1l );
  vpgl_fundamental_matrix<double> F1( C1r, C1l );

  vgl_homg_point_3d<double> p1w( .4, -0.1, 10 );
  vgl_homg_point_2d<double> p1r = C1r.project( p1w );
  vgl_homg_point_2d<double> p1l = C1l.project( p1w );

  vnl_vector_fixed<double,3> p1r_vnl( p1r.x(), p1r.y(), p1r.w() );
  vnl_vector_fixed<double,3> l1r_vnl = F1.get_matrix() * p1r_vnl;
  vgl_homg_line_2d<double> l1r( l1r_vnl[0], l1r_vnl[1], l1r_vnl[2] );

  TEST_NEAR( "constructor from cameras", vgl_distance( l1r, p1l ), 0, 1e-06 );

  // Test constructor from matrix:
  double random_list2[9] = { 1, 5, 0, -3, 2, 100, 50, -20, 1 };
  vnl_matrix_fixed<double,3,3> F2_vnl( random_list2 );
  vpgl_fundamental_matrix<double> F2( F2_vnl );
  vnl_svd<double> svd2( F2.get_matrix().as_matrix() );
  TEST_NEAR( "constructor from matrix", svd2.W(2), 0, 1e-06 );

  // Test epipole finder:
  vgl_homg_point_2d<double> e1r, e1l;
  F1.get_epipoles( e1r, e1l );
  vnl_vector_fixed<double,3> e1r_vnl( e1r.x(), e1r.y(), e1r.w() );
  vnl_vector_fixed<double,3> e1l_vnl( e1l.x(), e1l.y(), e1l.w() );
  vnl_vector_fixed<double,3> z= F1.get_matrix() * e1r_vnl;
  TEST_NEAR( "right epipole finder", z[0]+z[1]+z[2], 0, 1e-06 );
  z= F1.get_matrix().transpose() * e1l_vnl;
  TEST_NEAR( "left epipole finder", z[0]+z[1]+z[2], 0, 1e-06 );

  // Test epipolar line finder:
  vgl_homg_point_3d<double> p2w( -4, 1, 0 );
  vgl_homg_point_2d<double> p2r = C1r.project( p2w );
  vgl_homg_point_2d<double> p2l = C1l.project( p2w );
  TEST_NEAR( "right epipolar line finder",
             vgl_distance( F1.r_epipolar_line( p2l ), p2r ), 0, 1e-06 );
  TEST_NEAR( "left epipolar line finder",
             vgl_distance( F1.l_epipolar_line( p2r ), p2l ), 0, 1e-06 );

  // Test camera extraction:
  double random_list2l[12] = { 5, 6, 40, -1, 15, 8, 5, -1, 10, 3, 7, 9 };
  vpgl_proj_camera<double> C2l( random_list2l );
  vpgl_proj_camera<double> C2r;
  vpgl_fundamental_matrix<double> F3( C2r, C2l );
  vpgl_proj_camera<double> C2l_est =
    F3.extract_left_camera( vnl_vector_fixed<double,3>(0,0,0), 1 );
  vpgl_fundamental_matrix<double> F3_est( C2r, C2l_est );
  std::cerr << "\nTrue fundamental matrix: " << F3.get_matrix() << '\n'
           << "Estimated fundamental matrix: " << F3_est.get_matrix() << '\n';
  TEST_NEAR( "left camera extraction",
             F3.get_matrix()(0,0)/F3_est.get_matrix()(0,0) -
             F3.get_matrix()(0,1)/F3_est.get_matrix()(0,1), 0, 1e-06 );

  // Test camera extraction with point correspondences.
  std::vector< vgl_point_3d<double> > p3w;
  std::vector< vgl_point_2d<double> > p3i;
  p3w.emplace_back( 1, 10, 3 );
  p3w.emplace_back( -5, 3, -4 );
  p3w.emplace_back( 3, -8, 1 );
  p3i.reserve(p3w.size());
for (const auto & i : p3w)
    p3i.emplace_back(
      C2l.project( vgl_homg_point_3d<double>(i) ) );
  vpgl_proj_camera<double> C2l_est2 =
    F3.extract_left_camera( p3w, p3i );
  std::cerr << "\nTrue camera matrix: " << C2l.get_matrix() << '\n'
           << "Estimated camera matrix: " << C2l_est2.get_matrix() << '\n'
           << C2l.project( vgl_homg_point_3d<double>(p3w[1]) ) << '\n'
           << C2l_est.project( vgl_homg_point_3d<double>(p3w[1]) ) << '\n';
  TEST_NEAR( "left camera extraction from correspondences",
             vgl_distance( p3i[0], vgl_point_2d<double>(
                C2l_est2.project( vgl_homg_point_3d<double>(p3w[0]) ) ) ), 0, 5.0 );

  // Test computation of a Fundamental matrix from an Essential matrix:
  vnl_double_3x3 K1_;
  K1_[0][0] = 880;   K1_[0][1] = 0;    K1_[0][2] = 330;
  K1_[1][0] = 0;     K1_[1][1] = 880;  K1_[1][2] = 244;
  K1_[2][0] = 0;     K1_[2][1] = 0;    K1_[2][2] = 1;
  vpgl_calibration_matrix<double> K1(K1_);

  vnl_double_3x3 K2_;
  K2_[0][0] = 600;   K2_[0][1] = 0;    K2_[0][2] = 320;
  K2_[1][0] = 0;     K2_[1][1] = 600;  K2_[1][2] = 240;
  K2_[2][0] = 0;     K2_[2][1] = 0;    K2_[2][2] = 1;
  vpgl_calibration_matrix<double> K2(K2_);

  //Test with a rotation of 30 degrees around the y axis
  vgl_h_matrix_3d<double> Rh;
  Rh.set_identity();
  vnl_vector_fixed<double,3> ax(0,1,0), cv(10, 0, 0), t;
  // vgl_point_3d<double> c(10,0,0);  //Camera center
  Rh.set_rotation_about_axis(ax, 0.5236); //Rotation matrix
  vnl_double_3x3 R = Rh.get_upper_3x3_matrix();
  t = -R*cv;

  vpgl_essential_matrix<double> E(skew_symmetric(t[0],t[1],t[2])*R);

  vpgl_fundamental_matrix<double> F4(K1, K2, E);
  vpgl_fundamental_matrix<double> Fi = actual_f_matrix();

  vnl_double_3x3 error = Fi.get_matrix()-F4.get_matrix();
  TEST_NEAR("Construct fundamental matrix from essential matrix",
            error.frobenius_norm(), 0, 1);
}

TESTMAIN(test_fundamental_matrix);
