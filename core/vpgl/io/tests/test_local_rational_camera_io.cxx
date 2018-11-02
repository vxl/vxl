#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/io/vpgl_io_local_rational_camera.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>

static void test_local_rational_camera_io()
{
  //Rational polynomial coefficients
  std::vector<double> neu_u(20,0.0), den_u(20,0.0), neu_v(20,0.0), den_v(20,0.0);
  neu_u[0]=0.1; neu_u[10]=0.071; neu_u[7]=0.01;  neu_u[9]=0.3;
  neu_u[15]=1.0; neu_u[18]=1.0, neu_u[19]=0.75;

  den_u[0]=0.1; den_u[10]=0.05; den_u[17]=0.01; den_u[9]=1.0;
  den_u[15]=1.0; den_u[18]=1.0; den_u[19]=1.0;

  neu_v[0]=0.02; neu_v[10]=0.014; neu_v[7]=0.1; neu_v[9]=0.4;
  neu_v[15]=0.5; neu_v[18]=0.01; neu_v[19]=0.33;

  den_v[0]=0.1; den_v[10]=0.05; den_v[17]=0.03; den_v[9]=1.0;
  den_v[15]=1.0; den_v[18]=0.3; den_v[19]=1.0;
  //Scale and offsets
  double sx = 50.0, ox = 150.0;
  double sy = 125.0, oy = 100.0;
  double sz = 5.0, oz = 10.0;
  double su = 1000.0, ou = 500;
  double sv = 500.0, ov = 200;
  vpgl_rational_camera<double> rat_cam(neu_u, den_u, neu_v, den_v,
                                       sx, ox, sy, oy, sz, oz,
                                       su, ou, sv, ov);

  vpgl_lvcs lvcs(33.4447732, -114.3085932, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vpgl_local_rational_camera<double> lrat_cam(lvcs, rat_cam);

  vsl_b_ofstream bp_out("test_local_rational_camera_io.tmp");
  TEST("Created test_local_rational_camera_io.tmp for writing",(!bp_out), false);
  vsl_b_write(bp_out, lrat_cam);
  bp_out.close();

  vsl_b_ifstream bp_in("test_local_rational_camera_io.tmp");
  TEST("Opened test_local_rational_camera_io.tmp for reading",(!bp_in), false);

  vpgl_local_rational_camera<double> lrat_cam_r;
  vsl_b_read(bp_in, lrat_cam_r);
  bp_in.close();
  TEST("lvcs is equal", lrat_cam_r.lvcs(), lvcs);
}

TESTMAIN(test_local_rational_camera_io);
