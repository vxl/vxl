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
  double sx = 0.02, ox = 150.0;
  double sy = 0.008, oy = 100.0;
  double sz = 0.2, oz = 10.0;
  double su = 0.001, ou = 500;
  double sv = 0.002, ov = 200;
  vpgl_rational_camera<double> rcam(neu_u, den_u, neu_v, den_v, 
                                    sx, ox, sy, oy, sz, oz,
                                    su, ou, sv, ov);

  vnl_double_2 image_point;
  vnl_double_3 initial_guess, world_point; 
  vnl_double_4 plane;
  image_point[0]=1276.68;   image_point[1]=397.414; 
  initial_guess[0]=150.0; initial_guess[1]=100.0; initial_guess[2]=10;
  plane[0]=0; plane[1]=0; plane[2]=1.0; plane[3]=-15.0;
 
  vcl_cout << "Initial Guess( " << initial_guess << ")\n";

  bool success = vpgl_backproject::bproj_plane(rcam, image_point, plane,
                                               initial_guess, world_point);

  vcl_cout << "Solution( " << world_point << ")\n";

  TEST_NEAR("test simple backprojection", (world_point[0]-200+world_point[1]-225+world_point[2]-15) , 0, 0.01);
  // Test with a plane not oriented on one of the axes
  
  vgl_point_2d<double> img_pt_0(1250.0, 365.0), img_pt_1(1047.62, 378.572),
    img_pt_2(1276.68, 397.414);
  //A plane through three of the known projection points
  vgl_point_3d<double> p0(150, 100, 10), p1(200, 100, 10), p2(200, 225, 15),
    iguess(150, 100, 10), wp0, wp1, wp2;
  vgl_plane_3d<double> pl3(p0, p1, p2);
  success = true;
  success = success && vpgl_backproject::bproj_plane(rcam, img_pt_0, pl3,
                                                     iguess, wp0);
  
  success = success && vpgl_backproject::bproj_plane(rcam, img_pt_1, pl3,
                                                     p1, wp1);

  success = success && vpgl_backproject::bproj_plane(rcam, img_pt_2, pl3,
                                                     iguess, wp2);
  vcl_cout << "solved world point 0( " << wp0 << ") vs (150,100,10)\n";
  vcl_cout << "solved world point 1( " << wp1 << ") vs (200,100,10)\n";
  vcl_cout << "solved world point 2( " << wp2 << ") vs (200,225,20)\n";
  TEST("test projection on Y-Z plane", success, true);
}

TESTMAIN(test_backproject);
