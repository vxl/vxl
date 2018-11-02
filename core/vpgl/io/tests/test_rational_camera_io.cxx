#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/io/vpgl_io_rational_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>

static void test_rational_camera_io()
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

  vsl_b_ofstream bp_out("test_rational_camera_io.tmp");
  TEST("Created test_rational_camera_io.tmp for writing",(!bp_out), false);
  vsl_b_write(bp_out, rat_cam);
  bp_out.close();

  vsl_b_ifstream bp_in("test_rational_camera_io.tmp");
  TEST("Opened test_rational_camera_io.tmp for reading",(!bp_in), false);

  vpgl_rational_camera<double> rat_cam_r;
  vsl_b_read(bp_in, rat_cam_r);
  bp_in.close();

  std::vector<std::vector<double> > coefs = rat_cam_r.coefficients();
  std::vector<double> neu_u_r = coefs[vpgl_rational_camera<double>::NEU_U],
    den_u_r = coefs[vpgl_rational_camera<double>::DEN_U];
  std::vector<double> neu_v_r = coefs[vpgl_rational_camera<double>::NEU_V],
    den_v_r = coefs[vpgl_rational_camera<double>::DEN_V];
  bool eq = (neu_u_r == neu_u) && (den_u_r == den_u);
  eq = eq && (neu_v_r == neu_v) && (den_v_r == den_v);
  TEST("coef recovery from binary read", eq , true );
  bool seq = rat_cam_r.scale(vpgl_rational_camera<double>::X_INDX)==sx;
  seq = seq && rat_cam_r.scale(vpgl_rational_camera<double>::Y_INDX)==sy;
  seq = seq && rat_cam_r.scale(vpgl_rational_camera<double>::Z_INDX)==sz;
  TEST("scale recovery from binary read", seq , true );
  bool oeq = rat_cam_r.offset(vpgl_rational_camera<double>::X_INDX)==ox;
  oeq = oeq && rat_cam_r.offset(vpgl_rational_camera<double>::Y_INDX)==oy;
  oeq = oeq && rat_cam_r.offset(vpgl_rational_camera<double>::Z_INDX)==oz;
  TEST("offset recovery from binary read", oeq , true );
}

TESTMAIN(test_rational_camera_io);
