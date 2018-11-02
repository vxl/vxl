#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/io/vpgl_io_camera.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x4.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
static double camera_diff_norm(vpgl_perspective_camera<double> const& C0,
                               vpgl_perspective_camera<double> const& C1)
{
  vnl_matrix_fixed<double,3,4> dif = C0.get_matrix()- C1.get_matrix();
  return dif.fro_norm();
}

static void test_camera_io()
{
  std::cout << "Testing base camera io" << std::endl;
  vpgl_camera<double>* cam, *cam_r;
  //===========   proj_camera ==================
  // Some matrices for testing.
  double random_list[12] = { 10.6, 1.009, .676, .5, -13, -10, 8, 5, 88, -2, -100, 11 };
  vnl_double_3x4 random_matrix( random_list );
  cam = new vpgl_proj_camera<double>(random_matrix);
  vsl_b_ofstream bp_outp("test_proj_camera_io.tmp");
  vsl_b_write(bp_outp, cam);
  bp_outp.close();

  vsl_b_ifstream bp_inp("test_proj_camera_io.tmp");
  vsl_b_read(bp_inp, cam_r);
  bp_inp.close();
  vpl_unlink("test_proj_camera_io.tmp");
  auto* pcam_r =
    dynamic_cast<vpgl_proj_camera<double>*>(cam_r);
  if (!pcam_r) {
    TEST("cast to proj_camera", false, true);
  }
  else {
    TEST("recovered proj_camera", pcam_r->get_matrix(), random_matrix );
  }
  //  delete cam_r; cam_r = 0;
  //================= test smart pointer io ============
  vpgl_camera_double_sptr cam_sptr = cam, cam_r_sptr=nullptr;
  vsl_b_ofstream bp_outps("test_camera_sptr_io.tmp");
  vsl_b_write(bp_outps, cam_sptr);
  bp_outps.close();

  vsl_b_ifstream bp_inps("test_camera_sptr_io.tmp");
  vsl_b_read(bp_inps, cam_r_sptr);
  bp_inp.close();
  vpl_unlink("test_camera_sptr_io.tmp");

  if (!cam_r_sptr)
    TEST("camera sptr read ", false, true);
  else {
    auto* local_pcam_r =
    dynamic_cast<vpgl_proj_camera<double>*>(cam_r_sptr.as_pointer());
    if (!local_pcam_r) {
      TEST("sptr cast to proj camera", false, true);
    }
    else {
      TEST("recovered proj_camera from sptr", local_pcam_r->get_matrix(), random_matrix );
    }
  }
  //delete cam; cam = 0;
  //===========   perspective_camera ==================
  double data[] = { 2000,    0, 512,
                    0, 2000, 384,
                    0,    0,   1 };
  vpgl_calibration_matrix<double> K = vnl_double_3x3(data);
  vgl_homg_point_3d<double>center(0,0,-10.0);

  // rotation angle in radians
  double theta = vnl_math::pi_over_4; // 45 degrees
  vnl_double_3 axis(0.0, 1.0, 0.0);
  vgl_h_matrix_3d<double> R;
  R.set_identity().set_rotation_about_axis(axis, theta);
  vpgl_perspective_camera<double>* percam =
    new vpgl_perspective_camera<double>(K, center, vgl_rotation_3d<double>(R));
  cam = percam;
  vsl_b_ofstream bp_outper("test_perspective_camera_io.tmp");
  vsl_b_write(bp_outper, cam);
  bp_outper.close();

  vsl_b_ifstream bp_inper("test_perspective_camera_io.tmp");
  vsl_b_read(bp_inper, cam_r);
  bp_inper.close();
  vpl_unlink("test_perspective_camera_io.tmp");

  auto* percam_r =
    dynamic_cast<vpgl_perspective_camera<double>*>(cam_r);
  if (!percam_r) {
    TEST("cast to perspective_camera", false, true);
  }
  else {
    double er = camera_diff_norm(*percam, *percam_r);
    TEST_NEAR("recovered perspective_camera", er, 0.0, 1e-3);
  }
  //  delete cam; cam = 0; delete cam_r; cam_r = 0;
  //===========   affine_camera ==================
  random_list[8]=0.0; random_list[9]=0.0;
  random_list[10]=0.0;  random_list[11]=1.0;
  auto* acam =
    new vpgl_affine_camera<double>( random_matrix );
  acam->set_viewing_distance(1000.0);
  cam = acam;

  vsl_b_ofstream bp_outa("test_affine_camera_io.tmp");
  vsl_b_write(bp_outa, cam);
  bp_outa.close();

  vsl_b_ifstream bp_ina("test_affine_camera_io.tmp");
  vsl_b_read(bp_ina, cam_r);
  bp_ina.close();
  vpl_unlink("test_affine_camera_io.tmp");

  auto* acam_r =
    dynamic_cast<vpgl_affine_camera<double>*>(cam_r);
  if (!acam_r) {
    TEST("cast to affine_camera", false, true);
  }
  else {
    TEST("recovered affine_camera", *acam, *acam_r);
  }

  //delete cam; cam = 0; delete cam_r; cam_r = 0;

  //===========   rational_camera ==================

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
  auto* rcam =
    new vpgl_rational_camera<double>(neu_u, den_u, neu_v, den_v,
                                     sx, ox, sy, oy, sz, oz,
                                     su, ou, sv, ov);
  cam = rcam;
  vsl_b_ofstream bp_outr("test_rational_camera_io.tmp");
  vsl_b_write(bp_outr, cam);
  bp_outr.close();

  vsl_b_ifstream bp_inr("test_rational_camera_io.tmp");
  vsl_b_read(bp_inr, cam_r);
  bp_inr.close();
  vpl_unlink("test_rational_camera_io.tmp");
  auto* rcam_r =
    dynamic_cast<vpgl_rational_camera<double>*>(cam_r);
  if (!rcam_r) {
    TEST("cast to rational_camera", false, true);
  }
  else {
    TEST("recovered rational_camera", *rcam, *rcam_r);
  }

  //  delete cam_r; cam_r = 0;
  //===========   local_rational_camera ==================

  vpgl_lvcs lvcs(33.4447732, -114.3085932, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  auto* lrcam =
    new vpgl_local_rational_camera<double>(lvcs, *rcam);
  delete cam;
  cam = lrcam;
  vsl_b_ofstream bp_outlr("test_local_rational_camera_io.tmp");
  vsl_b_write(bp_outlr, cam);
  bp_outlr.close();

  vsl_b_ifstream bp_inlr("test_local_rational_camera_io.tmp");
  vsl_b_read(bp_inlr, cam_r);
  bp_inlr.close();
  vpl_unlink("test_local_rational_camera_io.tmp");
  auto* lrcam_r =
    dynamic_cast<vpgl_local_rational_camera<double>*>(cam_r);
  if (!lrcam_r) {
    TEST("cast to local_rational_camera", false, true);
  }
  else {
    TEST("recovered local_rational_camera", *lrcam, *lrcam_r);
  }
  //  delete cam; cam = 0; delete cam_r; cam_r = 0;
}

TESTMAIN(test_camera_io);
