#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vpgl/vpgl_fundamental_matrix.h>
#include <vpgl/vpgl_reg_fundamental_matrix.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_distance.h>

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
  vcl_cerr << "\nTrue fundamental matrix: " << F3.get_matrix() << '\n'
           << "Estimated fundamental matrix: " << F3_est.get_matrix() << '\n';
  TEST_NEAR( "left camera extraction",
    F3.get_matrix()(0,0)/F3_est.get_matrix()(0,0) -
    F3.get_matrix()(0,1)/F3_est.get_matrix()(0,1), 0, 1e-06 );

  // Test camera extraction with point correspondences.
  vcl_vector< vgl_point_3d<double> > p3w;
  vcl_vector< vgl_point_2d<double> > p3i;
  p3w.push_back( vgl_point_3d<double>( 1, 10, 3 ) );
  p3w.push_back( vgl_point_3d<double>( -5, 3, -4 ) );
  p3w.push_back( vgl_point_3d<double>( 3, -8, 1 ) );
  for ( unsigned int i = 0; i < p3w.size(); ++i )
    p3i.push_back( vgl_point_2d<double>(
      C2l.project( vgl_homg_point_3d<double>(p3w[i]) ) ) );
  vpgl_proj_camera<double> C2l_est2 =
    F3.extract_left_camera( p3w, p3i );
  vcl_cerr << "\nTrue camera matrix: " << C2l.get_matrix() << '\n'
           << "Estimated camera matrix: " << C2l_est2.get_matrix() << '\n'
           << C2l.project( vgl_homg_point_3d<double>(p3w[1]) ) << '\n'
           << C2l_est.project( vgl_homg_point_3d<double>(p3w[1]) ) << '\n';
  TEST_NEAR( "left camera extraction from correspondences",
    vgl_distance( p3i[0], vgl_point_2d<double>( C2l_est2.project(
      vgl_homg_point_3d<double>(p3w[0]) ) ) ), 0, 4.0 );

  // Test registered fundamental matrix construction:
  vgl_point_2d<double> p3r(0,2);
  vgl_point_2d<double> p3l(-4,7);
  vpgl_reg_fundamental_matrix<double> F4(p3r,p3l);
  TEST_NEAR( "registered fundamental_matrix", dot_product(
    vnl_vector_fixed<double,3>(p3l.x(),p3l.y(),1),
    F4.get_matrix()*vnl_vector_fixed<double,3>(p3r.x(),p3r.y(),1)), 0, 1e-06 );
}

TESTMAIN(test_fundamental_matrix);
