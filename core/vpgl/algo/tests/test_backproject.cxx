#include <iostream>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_backproject()
{
  //Make the rational camera
  //Rational polynomial coefficients
  std::vector<double> neu_u(20,0.0), den_u(20,0.0), neu_v(20,0.0), den_v(20,0.0);
  neu_u[0]=0.1; neu_u[10]=.071; neu_u[ 7]=.01; neu_u[9]=0.3; neu_u[15]=1.0; neu_u[18]=1.0, neu_u[19]=.75;
  den_u[0]=0.1; den_u[10]=0.05; den_u[17]=.01; den_u[9]=1.0; den_u[15]=1.0; den_u[18]=1.0; den_u[19]=1.0;
  neu_v[0]=.02; neu_v[10]=.014; neu_v[ 7]=0.1; neu_v[9]=0.4; neu_v[15]=0.5; neu_v[18]=.01; neu_v[19]=.33;
  den_v[0]=0.1; den_v[10]=0.05; den_v[17]=.03; den_v[9]=1.0; den_v[15]=1.0; den_v[18]=0.3; den_v[19]=1.0;
  //Scale and offsets
  double sx = 50.0,  ox = 150.0;
  double sy = 120.0, oy = 100.0;
  double sz = 5.0,   oz = 10.0;
  double su = 1000.0,ou = 500.0;
  double sv = 400.0, ov = 200.0;
  vpgl_rational_camera<double> rcam(neu_u, den_u, neu_v, den_v,
                                    sx, ox, sy, oy, sz, oz,
                                    su, ou, sv, ov);

  vgl_point_3d<double> p0(150, 100, 10), p1(200, 100, 10), p2(200, 225, 15),
                       p3(319.179226838, 295.970213457, 10),
                       p4(339.087727547, 100, 16.0927455324);
  vgl_point_2d<double> test_point0 = rcam.project(p0);
  vgl_point_2d<double> test_point1 = rcam.project(p1);
  vgl_point_2d<double> test_point2 = rcam.project(p2);
  vgl_point_2d<double> test_point3 = rcam.project(p3); // equals test_point0 !!!
  vgl_point_2d<double> test_point4 = rcam.project(p4); // equals test_point0 !!!

  std::cout << "Projection of " << p0 << "is " << test_point0 << '\n'
           << "Projection of " << p1 << "is " << test_point1 << '\n'
           << "Projection of " << p2 << "is " << test_point2 << '\n'
           << "Projection of " << p3 << "is " << test_point3 << '\n'
           << "Projection of " << p4 << "is " << test_point4 << '\n';

  vnl_double_2 image_point(1250, 332);
  vgl_point_2d<double> img_pt(1250, 332);
  TEST_NEAR("test projection p0", (img_pt-test_point0).length(), 0, 1e-8);
  TEST_NEAR("test projection p3", (img_pt-test_point3).length(), 0, 1e-8);
  TEST_NEAR("test projection p4", (img_pt-test_point4).length(), 0, 1e-8);

  vnl_double_4 plane(0.0, 0.0, 1.0, -10.0); // plane z=10

  vnl_double_3 initial_guess(200.0, 150.0, 10.0);
  std::cout << "Initial X-Y Guess (" << initial_guess << ")\n";

  vnl_double_3 world_point;
  bool success = vpgl_backproject::bproj_plane(rcam, image_point, plane,
                                               initial_guess, world_point);

  std::cout << "X-Y Solution ("<< world_point <<") should be "<<p3<< '\n';
  TEST("X-Y plane backprojection convergence", success, true);
  vgl_point_3d<double> wp; wp.set(world_point[0],world_point[1],world_point[2]);
  // beware: both p0 and p3 project to image_point
  TEST_NEAR("test simple backprojection x-y plane", std::min((wp-p0).length(), (wp-p3).length()), 0, 1e-8);

  plane = vnl_double_4(1.0, 0.0, 0.0, -150.0); // plane x=150
  initial_guess = vnl_double_3(150.0, 150.0, 15.0);
  std::cout << "Initial Y-Z Guess (" << initial_guess << ")\n";
  success = vpgl_backproject::bproj_plane(rcam, image_point, plane,
                                          initial_guess, world_point);
  std::cout << "Y-Z Solution ("<< world_point <<") should be "<<p0<< '\n';
  TEST("Y-Z plane backprojection convergence", success, true);
  wp.set(world_point[0],world_point[1],world_point[2]);
  TEST_NEAR("test simple backprojection y-z plane", std::min((wp-p0).length(), (wp-p3).length()), 0, 1e-8);

  plane = vnl_double_4(0.0, 1.0, 0.0, -100.0); // plane y=100
  initial_guess = vnl_double_3(125.0, 100.0, 8.0);
  std::cout << "Initial X-Z Guess (" << initial_guess << ")\n";
  success = vpgl_backproject::bproj_plane(rcam, image_point, plane,
                                          initial_guess, world_point);
  std::cout << "X-Z Solution ("<< world_point <<") should be "<<p4<< '\n';
  TEST("X-Z plane backprojection convergence", success, true);
  wp.set(world_point[0],world_point[1],world_point[2]);
  // beware: both p0 and p4 project to image_point
  TEST_NEAR("test simple backprojection x-z plane", std::min((wp-p0).length(), (wp-p4).length()), 0, 1e-8);

  // Test with a plane not oriented on one of the axes

  img_pt = vgl_point_2d<double>(1199.1003963, 346.589238723);

  //A plane through three of the known projection points
  vgl_plane_3d<double> pl3(p0, p1, p2);
  vgl_point_3d<double> iguess(150, 100, 10);
  vgl_point_3d<double> correct(550.0/3, 425.0/3, 35.0/3);
  success = vpgl_backproject::bproj_plane(rcam, img_pt, pl3, iguess, wp);
  TEST("arbitrary plane backprojection convergence", success, true);
  TEST_NEAR("test backprojection on arbitrary plane", (wp-correct).length(), 0, 1e-8);
}

TESTMAIN(test_backproject);
