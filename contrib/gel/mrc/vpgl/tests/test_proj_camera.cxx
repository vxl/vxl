
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vpgl/vpgl_proj_camera.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_distance.h> 

static void test_proj_camera()
{

  // Some matrices for testing.
  vnl_matrix_fixed<double,3,4> identity_camera( 0.0 );
  identity_camera(0,0) = identity_camera(1,1) = identity_camera(2,2) = 1;
  double random_list[12] = { 1, 15, 9, -1, 2, -6, -9, 7, -5, 6, 10, 0 };
  vnl_matrix_fixed<double,3,4> random_matrix( random_list );
  double random_list2[12] = { 10.6, 1.009, .676, .5, -13, -10, 8, 5, 88, -2, -100, 11 };
  vnl_matrix_fixed<double,3,4> random_matrix2( random_list2 );
  double random_list3[12] = { 8.6, 66, -.8, 14, -.8, -100, 9.9, 2.4, 7, -1, -18, 90 };
  vnl_matrix_fixed<double,3,4> random_matrix3( random_list3 );

  // Default constructor.
  vpgl_proj_camera<double> P1;
  TEST( "Default constructor", P1.get_matrix(), identity_camera );

  // Construct from array and vnl_fixed_matrix.
  vpgl_proj_camera<double> P2( random_list );
  TEST( "Array constructor", P2.get_matrix(), random_matrix );

  vpgl_proj_camera<double> P3( random_matrix2 );
  TEST( "vnl_fixed_matrix constructor", P3.get_matrix(), random_matrix2 );

  // Copy constructor and assignment.
  vpgl_proj_camera<double> P4( P2 );
  TEST( "Copy constructor", P4.get_matrix(), P2.get_matrix() );

  vpgl_proj_camera<double> P5; P5 = P2;
  TEST( "Assignment operator", P5.get_matrix(), P2.get_matrix() );

  P2 = P3;
  TEST( "Assignment makes deep copy", P5.get_matrix() != P3.get_matrix(), true );

  // Setters.
  P1.set_matrix( random_matrix );
  TEST( "set_matrix from vnl", P1.get_matrix(), random_matrix );
  P2.set_matrix( random_matrix2 );
  TEST( "set_matrix from array", P2.get_matrix(), random_matrix2 );

  // Point projection.
  vgl_homg_point_3d<double> x1( 1, 4, 5 );
  vgl_homg_point_2d<double> y1 = P1.project( x1 );
  vnl_vector_fixed<double,3> y2 = random_matrix*vnl_vector_fixed<double,4>(1,4,5,1);
  TEST( "point projection", y2(0)/y1.x() - y2(1)/y1.y(), 0.0 );

  // Line projection.
  vgl_homg_point_3d<double> x2( 4, 3, -10 );
  vgl_line_segment_3d<double> l1w( x1, x2 );
  vgl_line_segment_2d<double> l1i = P1.project( l1w );
  vgl_line_segment_2d<double> l1ib( P1.project( x1 ), P1.project( x2 ) );
  TEST_NEAR( "line projection", l1i.b() * l1ib.c(), l1ib.b() * l1i.c(), 1e-06 );

  // Point backprojection.
  vgl_homg_point_2d<double> y3( 100, 12 );
  vgl_homg_line_3d_2_points<double> l3 = P1.backproject( y3 );
  vgl_homg_point_2d<double> y3b = P1.project( l3.point_finite() );
  TEST_NEAR( "point backprojection", y3.x() * y3b.w(), y3b.x() * y3.w(), 1e-06 );

  // Plane backprojection.
  P2.set_matrix( random_matrix );
  vgl_homg_line_2d<double> l4(1,-2,3);
  vgl_homg_plane_3d<double> plane4 = P2.backproject( l4 );
  vgl_homg_point_3d<double> q1( 1, 0, 0, -plane4.a()/plane4.d() );
  vgl_homg_point_3d<double> q2( 0, 1, 0, -plane4.b()/plane4.d() );
  vgl_homg_point_3d<double> q3( 0, 0, 1, -plane4.c()/plane4.d() );
  vgl_homg_point_2d<double> q1i = P2.project(q1);
  vgl_homg_point_2d<double> q2i = P2.project(q2);
  vgl_homg_point_2d<double> q3i = P2.project(q3);
  TEST_NEAR( "plane backprojection1", 
    vgl_distance(l4,q1i)*vgl_distance(l4,q2i)*vgl_distance(l4,q3i), 0, 1e-06 );

  vgl_homg_line_2d<double> l5(-10,13,40);
  l5.get_two_points(q1i, q2i);
  vgl_homg_plane_3d<double> plane5a = P2.backproject( l5 );
  vgl_homg_plane_3d<double> plane5b( P2.backproject(q2i).point_finite(),
    P2.backproject(q1i).point_infinite(),P2.backproject(q2i).point_infinite() );
  TEST_NEAR( "plane backprojection2", 
    plane5a.a()*plane5b.d(), plane5b.a()*plane5a.d(), 1e-06 );

  // Test automatic SVD computation
  P1.svd();
  P1.set_matrix( random_matrix2 );
  TEST_NEAR( "automatic svd computation", random_matrix2(2,3), 
    P1.svd()->recompose()(2,3), 1e-06 );

  // Test get_canonical_h
  vpgl_proj_camera<double> P6( random_matrix );
  vgl_h_matrix_3d<double> H = get_canonical_h( P6 );
  vnl_matrix<double> I6 = P6.get_matrix() * H.get_matrix();
  TEST( "get_canonical_h",
    vcl_fabs(I6(0,0)*I6(1,1)*I6(2,2)-1) < 1e-06 && 
    vcl_fabs(I6(1,0)*I6(2,0)*I6(0,1)*I6(2,1)*I6(0,2)*I6(1,2)*I6(0,3)*I6(1,3)*I6(2,3))< 1e-06,
    true );

  // Test camera center
  vgl_homg_point_2d<double> q6 = P6.project( P6.camera_center() );
  TEST_NEAR( "camera center computation", q6.x()*q6.y()*q6.w(), 0, 1e-06 );
  
  // Test pre-multiply
  double T1array[9] = {1,2,3,4,5,6,7,8,9};
  vnl_matrix_fixed<double,3,3> T1( T1array );
  P1.set_matrix( random_matrix3 );
  P1 = premultiply( P1, T1 );
  TEST_NEAR( "pre-multiply", P1.get_matrix()(1,2), (T1*random_matrix3)(1,2), 1e-06 );

  // Test post-multiply
  double T2array[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
  vnl_matrix_fixed<double,4,4> T2( T2array );
  P1.set_matrix( random_matrix2 );
  P1 = postmultiply( P1, T2 );
  TEST_NEAR( "post-multiply", P1.get_matrix()(1,0), (random_matrix2*T2)(1,0), 1e-06 );

  // Test automatic SVD computation
  P1.set_matrix( random_matrix2 );
  P1.svd();
  P1.set_matrix( random_matrix3 );
  TEST_NEAR( "automatic svd computation", random_matrix3(2,3), 
    P1.svd()->recompose()(2,3), 1e-06 );

}


TESTMAIN(test_proj_camera);
