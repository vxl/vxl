#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bpgl/algo/bpgl_construct_cameras.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

static void test_construct_cameras()
{
    vpgl_calibration_matrix<double> K;
    K.set_focal_length(100); K.set_principal_point( vgl_point_2d<double>(200,200) );
    vpgl_perspective_camera<double> P1; P1.set_calibration( K );
    vpgl_perspective_camera<double> P2; P2.set_calibration( K );
    vgl_rotation_3d<double> R_true(.1, -.2, .2);
    vgl_point_3d<double> t_true( -.1, .2, .3 );
    P2.set_camera_center( t_true );
    P2.set_rotation( R_true );

    std::vector< vgl_point_2d<double> > points1, points2;
    for (int i = 0; i < 8; i++)
    {
      vgl_homg_point_3d<double> wp;
      if ( i == 0 ) wp.set( 1, 2, 3 );
      if ( i == 1 ) wp.set( -2, 4, -2 );
      if ( i == 2 ) wp.set( -7, 6, 3 );
      if ( i == 3 ) wp.set( 3, 8, -5 );
      if ( i == 4 ) wp.set( 5, 4, 1 );
      if ( i == 5 ) wp.set( -2, -6, -1 );
      if ( i == 6 ) wp.set( 0, 2, 1 );
      if ( i == 7 ) wp.set( -1, -2, 1 );
      vgl_homg_point_2d<double> p1 = P1.project(wp);
      vgl_homg_point_2d<double> p2 = P2.project(wp);
      vgl_point_2d<double> p1h( p1.x()/p1.w(), p1.y()/p1.w() );
      vgl_point_2d<double> p2h( p2.x()/p2.w(), p2.y()/p2.w() );
      points1.push_back( p1h );
      points2.push_back( p2h );
    }

    bpgl_construct_cameras testcase(points1,points2,&K);
    testcase.construct();

    std::cout << "\nTrue rotation:\n" << R_true.as_matrix()
             << "\n\nEstimated rotation:\n"
             << testcase.get_camera2().get_rotation().as_matrix()
             << '\n';

    vgl_point_3d<double> t_est = testcase.get_camera2().get_camera_center();
    double r1 = t_true.x()/t_est.x(),
           r2 = t_true.y()/t_est.y(),
           r3 = t_true.z()/t_est.z();
    std::cout << "\nTrue camera center:\n" << t_true
             << "\n\nEstimated camera center (up to scale):\n" << t_est << '\n';

    TEST_NEAR( "rotation matrix equivalent",
               (testcase.get_camera2().get_rotation().as_matrix()-R_true.as_matrix()).frobenius_norm(), 0, .1 );
    TEST_NEAR( "camera center proper direction",
               std::abs(r1-r2)+std::abs(r2-r3)+std::abs(r3-r1), 0, .1 );
    TEST( "camera center proper sign", r1 > 0 && r2 > 0 && r3 > 0, true );
}

TESTMAIN(test_construct_cameras);
