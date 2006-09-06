#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <mrc/vpgl/algo/vpgl_construct_cameras.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_3x3.h>

static void test_construct_cameras()
{
  if( true ){
    vpgl_calibration_matrix<double> K;
    K.set_focal_length(100); K.set_principal_point( vgl_point_2d<double>(200,200) );
    vpgl_perspective_camera<double> P1; P1.set_calibration( K );
    vpgl_perspective_camera<double> P2; P2.set_calibration( K );
    double id[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    vgl_h_matrix_3d<double> R_true(id); R_true.set_rotation_euler(.1, -.2, .2);
    vgl_point_3d<double> t_true( -.1, .2, .3 );
    P2.set_camera_center( t_true );
    P2.set_rotation_matrix( R_true );

    vcl_vector< vgl_point_2d<double> > points1, points2;
    for( int i = 0; i < 8; i++ ){
      vgl_homg_point_3d<double> wp;
      if( i == 0 ) wp.set( 1, 2, 3 );
      if( i == 1 ) wp.set( -2, 4, -2 );
      if( i == 2 ) wp.set( -7, 6, 3 );
      if( i == 3 ) wp.set( 3, 8, -5 );
      if( i == 4 ) wp.set( 5, 4, 1 );
      if( i == 5 ) wp.set( -2, -6, -1 );
      if( i == 6 ) wp.set( 0, 2, 1 );
      if( i == 7 ) wp.set( -1, -2, 1 );
      vgl_homg_point_2d<double> p1 = P1.project(wp);
      vgl_homg_point_2d<double> p2 = P2.project(wp);
      vgl_point_2d<double> p1h( p1.x()/p1.w(), p1.y()/p1.w() ); 
      vgl_point_2d<double> p2h( p2.x()/p2.w(), p2.y()/p2.w() ); 
      points1.push_back( p1h );
      points2.push_back( p2h );
    }

    vpgl_construct_cameras testcase(points1,points2,&K);
    testcase.construct();

    vcl_cout << "\nTrue rotation:\n" << R_true.get_matrix() << '\n';
    vcl_cout << "\nEstimated rotation:\n" << 
      testcase.get_camera2().get_rotation_matrix().get_matrix() << '\n';

    vgl_point_3d<double> t_est = testcase.get_camera2().get_camera_center();
    double r1 = t_true.x()/t_est.x(), r2 = t_true.y()/t_est.y(), r3 = t_true.z()/t_est.z();
    vcl_cout<< "\nTrue camera center:\n" << t_true << '\n';
    vcl_cout<< "\nEstimated camera center (up to scale):\n" << t_est << '\n';

    TEST_NEAR( "rotation matrix equivalent", 
      (testcase.get_camera2().get_rotation_matrix().get_matrix()-R_true.get_matrix()).frobenius_norm(), 0, .1 );
    TEST_NEAR( "camera center proper direction", 
      abs(r1-r2)+abs(r2-r3)+abs(r3-r1), 0, .1 );
    TEST_NEAR( "camera center proper sign", 
      r1 > 0 && r2 > 0 && r3 > 0, true, 0 );
    
  }

}

TESTMAIN(test_construct_cameras);
