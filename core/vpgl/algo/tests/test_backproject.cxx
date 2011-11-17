#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>
static void test_backproject()
{
  //Make the rational camera
  //Rational polynomial coefficients
  vcl_vector<double> neu_u(20,0.0), den_u(20,0.0), neu_v(20,0.0), den_v(20,0.0);
  neu_u[0]=0.1; neu_u[10]=0.071; neu_u[7]=0.01;  neu_u[9]=0.3;
  neu_u[15]=1.0; neu_u[18]=1.0, neu_u[19]=0.75;

  den_u[0]=0.1; den_u[10]=0.05; den_u[17]=0.01; den_u[9]=1.0;
  den_u[15]=1.0; den_u[18]=1.0; den_u[19]=1.0;

  neu_v[0]=0.02; neu_v[10]=0.014; neu_v[7]=0.1; neu_v[9]=0.4;
  neu_v[15]=0.5; neu_v[18]=0.01; neu_v[19]=0.33;

  den_v[0]=0.1; den_v[10]=0.05; den_v[17]=0.03; den_v[9]=1.0;
  den_v[15]=1.0; den_v[18]=0.3; den_v[19]=1.0;
  //Scale and offsets
  double sx = 50, ox = 150.0;
  double sy = 120, oy = 100.0;
  double sz = 5, oz = 10.0;
  double su = 1000, ou = 500;
  double sv = 400, ov = 200;
  vpgl_rational_camera<double> rcam(neu_u, den_u, neu_v, den_v, 
                                    sx, ox, sy, oy, sz, oz,
                                    su, ou, sv, ov);

  vnl_double_2 image_point;
  vgl_point_2d<double> test_point0, test_point1, test_point2, test_point3;
  vnl_double_3 world_point, initial_guess;
  vgl_point_3d<double> wp, p0(150, 100, 10), p1(200, 100, 10), p2(200, 225, 15),
                       p3(183.3333, 141.66667, 11.66667), iguess(150, 100, 10);
  test_point0 = rcam.project(p0);
  test_point1 = rcam.project(p1);
  test_point2 = rcam.project(p2);
  test_point3 = rcam.project(p3);


  vcl_cout << "Projection of( " << p0 << ") is (" << test_point0 << ")\n"
           << "Projection of( " << p1 << ") is (" << test_point1 << ")\n"
           << "Projection of( " << p2 << ") is (" << test_point2 << ")\n"
           << "Projection of( " << p3 << ") is (" << test_point3 << ")\n";

  vnl_double_4 plane;
  image_point[0]=1250.0;   image_point[1]=332;
  initial_guess[0]=200.0; initial_guess[1]=150.0; initial_guess[2]=15.0;
  plane[0]=0; plane[1]=0; plane[2]=1.0; plane[3]=-10.0;
 
  vcl_cout << "Initial X-Y Guess( " << initial_guess << ")\n";

  bool success = vpgl_backproject::bproj_plane(rcam, image_point, plane,
                                               initial_guess, world_point);

  vcl_cout<< "X-Y Solution( "<< world_point <<") success is "<< success<< '\n';
  wp.set(world_point[0],world_point[1],world_point[2]); 
  TEST_NEAR("test simple backprojection x-y plane", (wp-p0).length(), 0, 1e-8);

  plane[0]=1.0; plane[1]=0; plane[2]=0.0; plane[3]=-150.0;
  vcl_cout << "Initial Y-Z Guess( " << initial_guess << ")\n";
  success = vpgl_backproject::bproj_plane(rcam, image_point, plane,
                                               initial_guess, world_point);
  vcl_cout << "Y-Z Solution( " << world_point << ") success is "<< success << '\n';
  wp.set(world_point[0],world_point[1],world_point[2]); 
  TEST_NEAR("test simple backprojection y-z plane", (wp-p0).length(), 0, 1e-8);

  initial_guess[0]=125.0; initial_guess[1]=110.0; initial_guess[2]=8.0;
  plane[0]=0.0; plane[1]=1.0; plane[2]=0.0; plane[3]=-100.0;
  vcl_cout << "Initial X-Z Guess( " << initial_guess << ")\n";
  success = vpgl_backproject::bproj_plane(rcam, image_point, plane,
                                               initial_guess, world_point);
  vcl_cout << "X-Z Solution( " << world_point << ") success is " << success << '\n';
  wp.set(world_point[0],world_point[1],world_point[2]); 
  TEST_NEAR("test simple backprojection x-z plane", (wp-p0).length(), 0, 1e-8);

  // Test with a plane not oriented on one of the axes

  vgl_point_2d<double> img_pt(1199.1, 346.589);

  //A plane through three of the known projection points
  vgl_plane_3d<double> pl3(p0, p1, p2);
  success = vpgl_backproject::bproj_plane(rcam, img_pt, pl3, iguess, wp);
  TEST("arbitrary plane backprojection convergence", success, true);
  TEST_NEAR("test backprojection on arbitrary plane", (wp-p3).length(), 0, 0.01);
}

TESTMAIN(test_backproject);
