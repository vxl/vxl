#include <iostream>
#include "testlib/testlib_test.h"
#if 0
#  include "testlib/testlib_root_dir.h"
#endif
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vnl/vnl_fwd.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_random.h"
#include "vgl/vgl_homg_point_3d.h"
#include "vgl/vgl_point_3d.h"
#include "vgl/vgl_point_2d.h"
#include "vgl/vgl_distance.h"
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_calibration_matrix_compute.h>
#include <vpgl/algo/vpgl_ray.h>
#include "vpgl/vpgl_proj_camera.h"
#include "vpgl/vpgl_affine_camera.h"
#include "vpgl/vpgl_perspective_camera.h"
#include "vpgl/vpgl_calibration_matrix.h"

static void
test_camera_compute_setup()
{
  // PART 1: Test the affine camera computation

  vnl_vector_fixed<double, 4> r1(1, 2, 3, 4);
  vnl_vector_fixed<double, 4> r2(-1, 4, -2, 0);
  vpgl_affine_camera<double> C1(r1, r2);
  std::vector<vgl_point_3d<double>> world_pts;
  world_pts.emplace_back(1, 0, -1);
  world_pts.emplace_back(6, 1, 2);
  world_pts.emplace_back(-1, -3, -2);
  world_pts.emplace_back(0, 0, 2);
  world_pts.emplace_back(2, -1, -5);
  world_pts.emplace_back(8, 1, -2);
  world_pts.emplace_back(-4, -4, 5);
  world_pts.emplace_back(-1, 3, 4);
  world_pts.emplace_back(1, 2, -7);
  std::vector<vgl_point_2d<double>> image_pts;
  image_pts.reserve(world_pts.size());
  for (const auto & world_pt : world_pts)
    image_pts.emplace_back(C1.project(vgl_homg_point_3d<double>(world_pt)));

  vpgl_affine_camera<double> C1e;
  vpgl_affine_camera_compute::compute(image_pts, world_pts, C1e);

  std::cerr << "\nTrue camera matrix:\n"
            << C1.get_matrix() << '\n'
            << "\nEstimated camera matrix:\n"
            << C1e.get_matrix() << '\n';
  TEST_NEAR("vpgl_affine_camera_compute:", (C1.get_matrix() - C1e.get_matrix()).frobenius_norm(), 0, 1);
}


void
test_perspective_compute()
{
  std::cout << "Test Perspective Compute\n";
  vnl_vector_fixed<double, 3> rv, trans;
  for (unsigned i = 0; i < 3; ++i)
    rv[i] = 0.9068996774314604; // axis along diagonal, rotation of 90 degrees
  vgl_rotation_3d<double> rr(rv);

  trans[0] = 10.0;
  trans[1] = 20.0;
  trans[2] = 30;

  vnl_matrix<double> Y(3, 5);
  Y[0][0] = 1.1;
  Y[1][0] = -0.05;
  Y[2][0] = 0.01;
  Y[0][1] = 0.02;
  Y[1][1] = 0.995;
  Y[2][1] = -0.1;
  Y[0][2] = -0.01;
  Y[1][2] = 0.04;
  Y[2][2] = 1.04;
  Y[0][3] = 1.15;
  Y[1][3] = 0.97;
  Y[2][3] = -0.1;
  Y[0][4] = 1.01;
  Y[1][4] = 1.03;
  Y[2][4] = 0.96;

  vnl_matrix<double> J(4, 6);
  for (unsigned c = 0; c < 5; ++c)
  {
    for (unsigned r = 0; r < 3; ++r)
      J[r][c] = Y[r][c];
    J[3][c] = 1.0;
  }
  J[0][5] = 0.5;
  J[1][5] = 1.0;
  J[2][5] = -0.5;
  J[3][5] = 1.0;

  vnl_matrix_fixed<double, 3, 3> pr = rr.as_matrix();
  vnl_matrix_fixed<double, 3, 4> P;
  for (unsigned r = 0; r < 3; ++r)
  {
    for (unsigned c = 0; c < 3; ++c)
      P[r][c] = pr[r][c];
    P[r][3] = trans[r];
  }
  // Project the 3-d points
  vnl_matrix<double> Z(2, 6);
  for (unsigned c = 0; c < 6; ++c)
  {
    vnl_vector_fixed<double, 4> vpr;
    for (unsigned r = 0; r < 4; ++r)
      vpr[r] = J[r][c];
    vnl_vector_fixed<double, 3> pvpr = P * vpr;
    for (unsigned r = 0; r < 2; ++r)
      Z[r][c] = pvpr[r] / pvpr[2];
  }
  std::cout << "Projected points\n " << Z << '\n';
  std::vector<vgl_point_2d<double>> image_pts;
  std::vector<vgl_point_3d<double>> world_pts;
  for (unsigned i = 0; i < 6; ++i)
  {
    vgl_point_2d<double> ip(Z[0][i], Z[1][i]);
    vgl_point_3d<double> wp(J[0][i], J[1][i], J[2][i]);
    image_pts.push_back(ip);
    world_pts.push_back(wp);
  }
  vpgl_calibration_matrix<double> K;
  vpgl_perspective_camera<double> pc;

  vpgl_perspective_camera_compute::compute(image_pts, world_pts, K, pc);
  std::cout << pc << '\n';
  vgl_point_3d<double> cc = pc.get_camera_center();
  TEST_NEAR("perspective camera from 6 points exact", cc.z(), -14.2265, 0.001);
}
// Tests the compute(world_pts, image_pts, camera) method in
// the vpgl_camera_compute class
static void
test_perspective_compute_direct_linear_transform()
{
  // Create the world points
  std::vector<vgl_point_3d<double>> world_pts;
  world_pts.emplace_back(1, 0, -1);
  world_pts.emplace_back(6, 1, 2);
  world_pts.emplace_back(-1, -3, -2);
  world_pts.emplace_back(0, 0, 2);
  world_pts.emplace_back(2, -1, -5);
  world_pts.emplace_back(8, 1, -2);

  // Come up with the projection matrix.
  vnl_matrix_fixed<double, 3, 4> proj;
  proj.set(0, 0, 0);
  proj.set(0, 1, 1);
  proj.set(0, 2, 0);
  proj.set(0, 3, 3);
  proj.set(1, 0, -1);
  proj.set(1, 1, 0);
  proj.set(1, 2, 0);
  proj.set(1, 3, 2);
  proj.set(2, 0, 0);
  proj.set(2, 1, 0);
  proj.set(2, 2, 1);
  proj.set(2, 3, 0);

  // Do the projection for each of the points
  std::vector<vgl_point_2d<double>> image_pts;
  for (auto & i : world_pts)
  {
    vnl_vector_fixed<double, 4> world_pt;
    world_pt[0] = i.x();
    world_pt[1] = i.y();
    world_pt[2] = i.z();
    world_pt[3] = 1.0;

    vnl_vector_fixed<double, 3> projed_pt = proj * world_pt;

    image_pts.emplace_back(projed_pt[0] / projed_pt[2], projed_pt[1] / projed_pt[2]);
  }

  // Calculate the projected points
  vpgl_perspective_camera<double> camera;
  double err;
  vpgl_perspective_camera_compute::compute_dlt(image_pts, world_pts, camera, err);

  TEST_NEAR("Small error.", err, 0, .1);

  // Check that it is close.
  for (unsigned int i = 0; i < world_pts.size(); i++)
  {
    double x, y;
    camera.project(world_pts[i].x(), world_pts[i].y(), world_pts[i].z(), x, y);

    TEST_NEAR("Testing that x coord is close", x, image_pts[i].x(), .001);
    TEST_NEAR("Testing that y coord is close", y, image_pts[i].y(), .001);
  }
}

static void
test_perspective_compute_ground()
{
  vpgl_calibration_matrix<double> trueK(1680, vgl_point_2d<double>(959.5, 539.5));
  vgl_rotation_3d<double> trueR(vnl_vector_fixed<double, 3>(1.87379, 0.0215981, -0.0331475));
  vgl_point_3d<double> trueC(14.5467, -6.71791, 4.79478);
  vpgl_perspective_camera<double> trueP(trueK, trueC, trueR);

  // generate some points on the ground
  std::vector<vgl_point_2d<double>> ground_pts;
  ground_pts.emplace_back(14.3256, 5.7912);
  ground_pts.emplace_back(14.3256, 9.4488);
  ground_pts.emplace_back(14.3256, 15.24);
  ground_pts.emplace_back(5.7404, 9.398);
  ground_pts.emplace_back(22.8092, 9.398);

  // project them to the image
  std::vector<vgl_point_2d<double>> image_pts;
  for (auto & ground_pt : ground_pts)
  {
    vgl_homg_point_3d<double> world_pt(ground_pt.x(), ground_pt.y(), 0, 1);

    vgl_point_2d<double> img_pt = trueP.project(world_pt);
    assert(img_pt.x() >= 0);
    assert(img_pt.x() <= 1920);
    assert(img_pt.y() >= 0);
    assert(img_pt.y() <= 1080);

    image_pts.push_back(img_pt);
  }

  vpgl_perspective_camera<double> P;
  P.set_calibration(trueK);

  bool did_compute = vpgl_perspective_camera_compute::compute(image_pts, ground_pts, P);

  TEST("Calibrate from ground<->image correspondences", did_compute, true);
  TEST_NEAR("   C", vgl_distance(trueC, P.get_camera_center()), 0, 1e-6);
  TEST_NEAR("   R", (trueR.as_matrix() - P.get_rotation().as_matrix()).frobenius_norm(), 0, 1e-6);
}

static void
test_calibration_compute_natural()
{
  vpgl_calibration_matrix<double> trueK(1680, vgl_point_2d<double>(959.5, 539.5));
  vgl_rotation_3d<double> trueR(vnl_vector_fixed<double, 3>(1.87379, 0.0215981, -0.0331475));
  vgl_point_3d<double> trueC(14.5467, -6.71791, 4.79478);
  vpgl_perspective_camera<double> trueP(trueK, trueC, trueR);

  // generate some points on the ground
  std::vector<vgl_point_2d<double>> ground_pts;
  ground_pts.emplace_back(14.3256, 5.7912);
  ground_pts.emplace_back(14.3256, 9.4488);
  ground_pts.emplace_back(14.3256, 15.24);
  ground_pts.emplace_back(5.7404, 9.398);
  ground_pts.emplace_back(22.8092, 9.398);

  // project them to the image
  std::vector<vgl_point_2d<double>> image_pts;
  for (auto & ground_pt : ground_pts)
  {
    vgl_homg_point_3d<double> world_pt(ground_pt.x(), ground_pt.y(), 0, 1);

    vgl_point_2d<double> img_pt = trueP.project(world_pt);
    assert(img_pt.x() >= 0);
    assert(img_pt.x() <= 1920);
    assert(img_pt.y() >= 0);
    assert(img_pt.y() <= 1080);

    image_pts.push_back(img_pt);
  }

  vpgl_calibration_matrix<double> K;
  bool did_compute = vpgl_calibration_matrix_compute::natural(image_pts, ground_pts, trueK.principal_point(), K);

  TEST("Calibrate natural intrinsics from correspondences", did_compute, true);
  TEST_NEAR("   K Discrepancy", (trueK.get_matrix() - K.get_matrix()).frobenius_norm(), 0, 1e-6);
}

static void
test_compute_affine()
{
  std::vector<vgl_point_3d<double>> pts_3d;
  pts_3d.emplace_back(31.191099166870, 121.919998168945, 53.835998535156);
  pts_3d.emplace_back(67.191101074219, 66.720397949219, 74.165496826172);
  pts_3d.emplace_back(54.891101837158, 97.920303344727, 66.359802246094);
  pts_3d.emplace_back(50.991100311279, 80.220397949219, 67.610298156738);
  pts_3d.emplace_back(28.791099548340, 67.320396423340, 67.759902954102);
  pts_3d.emplace_back(53.091098785400, 40.920299530029, 68.854202270508);
  pts_3d.emplace_back(43.791099548340, 49.620399475098, 67.872703552246);
  pts_3d.emplace_back(93.290100097656, 30.420299530029, 79.595397949219);
  pts_3d.emplace_back(94.790100097656, 52.020401000977, 74.720100402832);
  pts_3d.emplace_back(100.489997863770, 86.520401000977, 68.012802124023);
  pts_3d.emplace_back(91.790100097656, 102.419998168945, 68.957199096680);
  pts_3d.emplace_back(87.891098022461, 127.620002746582, 67.613098144531);
  pts_3d.emplace_back(31.491100311279, 107.220001220703, 60.077598571777);
  pts_3d.emplace_back(59.391101837158, 115.319999694824, 61.324699401855);
  pts_3d.emplace_back(59.991100311279, 133.320007324219, 62.057498931885);
  pts_3d.emplace_back(36.891101837158, 144.119995117188, 55.506599426270);
  pts_3d.emplace_back(18.891099929810, 111.419998168945, 57.615100860596);
  pts_3d.emplace_back(19.491100311279, 78.720397949219, 85.827499389648);
  pts_3d.emplace_back(26.991100311279, 35.220401763916, 83.151901245117);
  pts_3d.emplace_back(61.191101074219, 16.320400238037, 68.855102539063);
  pts_3d.emplace_back(127.489997863770, 32.220401763916, 96.971298217773);
  pts_3d.emplace_back(123.290000915527, 16.920299530029, 99.897903442383);
  pts_3d.emplace_back(122.089996337891, 92.220397949219, 69.267196655273);
  pts_3d.emplace_back(85.491096496582, 92.820396423340, 67.331802368164);
  vnl_vector_fixed<double, 4> row00, row01;
  row00[0] = 2.13641;
  row00[1] = 0.000728937;
  row00[2] = -0.103897;
  row00[3] = 253.827;
  row01[0] = -0.0107484;
  row01[1] = -2.13683;
  row01[2] = -0.229387;
  row01[3] = 648.49;
  vpgl_affine_camera<double> acam(row00, row01), fitted_acam;
  std::vector<vgl_point_2d<double>> pts_2d;
  for (const auto & i : pts_3d)
  {
    pts_2d.emplace_back(acam.project(i));
  }
  bool good = vpgl_affine_camera_compute::compute(pts_2d, pts_3d, fitted_acam);
  double er0 = 0.0;
  vnl_matrix_fixed<double, 3, 4> Mgt = acam.get_matrix(), Mfitted = fitted_acam.get_matrix();
  for (size_t r = 0; r < 2; ++r)
    for (size_t c = 0; c < 2; ++c)
    {
      er0 += fabs(Mgt[r][c] - Mfitted[r][c]);
    }
  good = good && er0 < 1.0e-6;
  TEST("compute affine camera from pts", good, true);

  // ==========================test ransac algorithm================================
  // realistic affine camera
  std::vector<std::vector<double>> A = {
    { 1.4008339964662238, 0.01379357608876183, 0.29176389136913616, 168.94607022009185 },
    { 0.21473016282540966, -1.0077720142913837, -1.01170300092531, 848.158895152373 },
    { 0.0, 0.0, 0.0, 1.0 }
  };


  vnl_matrix_fixed<double, 3, 4> m_a;

  for (size_t row = 0; row < 3; ++row)
    for (size_t col = 0; col < 4; ++col)
      m_a[row][col] = A[row][col];

  std::cout << "True Camera \n" << m_a << std::endl;
  vpgl_affine_camera<double> cam_A(m_a);

  // generate test correspondences
  vnl_random rand;
  std::vector<vgl_point_2d<double>> img_pts;
  std::vector<vgl_point_3d<double>> wld_pts;
  double x_min = 0.0, x_max = 500.0, inc = 50.0;
  double y_min = 0.0, y_max = 400.0;
  double z_min = 0.0, z_max = 300.0;
  for (size_t i = 0; i < 100; ++i)
  {
    double x = rand.drand32(x_min, x_max);
    double y = rand.drand32(y_min, y_max);
    double z = rand.drand32(z_min, z_max);
    vgl_point_3d<double> wld_pt(x, y, z);
    vgl_point_2d<double> img_pt = cam_A.project(wld_pt);
    wld_pts.push_back(wld_pt);
    img_pts.push_back(img_pt);
  }
  vpgl_affine_camera<double> Crsac;
  vpgl_affine_camera_compute::compute_robust_ransac(img_pts, wld_pts, Crsac);
  std::cout << "Test Camera \n" << Crsac.get_matrix() << std::endl;
  TEST_NEAR("vpgl_affine_camera_compute:", (Crsac.get_matrix() - m_a).frobenius_norm(), 0, 1);
  double img_sd = 1.5, wld_sd = 3;
  std::vector<vgl_point_2d<double>> noisy_img_pts;
  std::vector<vgl_point_3d<double>> noisy_wld_pts;
  for (size_t i = 0; i < 100; ++i)
  {
    if (i % 3 == 0)
    { // 33% noisy
      double u_noise = rand.drand32(-img_sd, img_sd);
      double v_noise = rand.drand32(-img_sd, img_sd);
      double x_noise = rand.drand32(-wld_sd, wld_sd);
      double y_noise = rand.drand32(-wld_sd, wld_sd);
      double z_noise = rand.drand32(-wld_sd, wld_sd);
      noisy_img_pts.emplace_back(img_pts[i].x() + u_noise, img_pts[i].y() + v_noise);
      noisy_wld_pts.emplace_back(wld_pts[i].x() + x_noise, wld_pts[i].y() + y_noise, wld_pts[i].z() + z_noise);
    }
    else
    {
      noisy_img_pts.push_back(img_pts[i]);
      noisy_wld_pts.push_back(wld_pts[i]);
    }
  }
  vpgl_affine_camera<double> Crsac_with_noise;
  vpgl_affine_camera_compute::compute_robust_ransac(noisy_img_pts, noisy_wld_pts, Crsac_with_noise);
  std::cout << "Test Camera With Noise \n" << Crsac_with_noise.get_matrix() << std::endl;
  TEST_NEAR("vpgl_affine_camera_compute: with noise", (Crsac_with_noise.get_matrix() - m_a).frobenius_norm(), 0, 1);
}


static void
test_compute_rational()
{
  double neu_u1[20] = { 8.96224e-005,  5.01302e-005, -7.67889e-006, -0.010342,    -6.89891e-005,
                        7.97337e-006,  0.00149824,   -1.38598e-005, 0.000188125,  0.980305,
                        -6.26076e-005, 2.9209e-006,  -0.0021416,    3.32719e-008, -0.000322074,
                        -7.1486e-005,  -2.9938e-007, 1.36383e-005,  0.023163,     0.0108915 };
  double den_u1[20] = { 7.47152e-007,
                        1.90129e-006,
                        -1.90195e-007,
                        -1.31851e-005,
                        1.99123e-006,
                        -3.32589e-008,
                        -3.46297e-005,
                        -1.27463e-007,
                        5.46123e-006,
                        -0.000542706,
                        1.32208e-006,
                        -6.82432e-008,
                        7.26265e-005,
                        3.35976e-008,
                        -3.78315e-006,
                        -0.00145714,
                        0,
                        -1.42542e-005,
                        -0.000437505,
                        1 };
  double neu_v1[20] = { -8.19635e-006, -4.83573e-005, 7.79866e-007, -0.00106039, -5.42129e-006, 0,
                        -0.00193506,   -6.93011e-006, 4.74326e-005, -0.132446,   -0.000200527,  -2.45944e-006,
                        0.00300919,    -5.85355e-005, 0.000148533,  -1.12081,    5.08764e-007,  -2.81721e-006,
                        0.0116155,     -0.00375141 };
  double den_v1[20] = { -6.9545e-007, -1.47008e-005, 2.14085e-007,  2.83534e-005,  -0.000119086,
                        2.32147e-006, -3.8376e-005,  -2.77014e-007, 5.07729e-007,  -0.00280166,
                        -0.000309513, 9.6094e-006,   -0.000178701,  -2.26873e-007, 1.61618e-006,
                        -0.00102174,  -2.21943e-008, 5.21377e-005,  0.000211917,   1 };

  // Scale and offsets
  double sx = 0.117798, ox = 44.2834;
  double sy = 0.114598, oy = 33.2609;
  double sz = 526.733, oz = 36.9502;
  double su = 14106, ou = 13785;
  double sv = 15402, ov = 15216;

  vpgl_rational_camera<double> rcam(neu_u1, den_u1, neu_v1, den_v1, sx, ox, sy, oy, sz, oz, su, ou, sv, ov);
  std::vector<vgl_point_2d<double>> image_pts;
  std::vector<vgl_point_3d<double>> ground_pts;
  size_t n_points = 1000;
  vnl_random rng;
  for (unsigned i = 0; i < n_points; i++)
  {
    double x = 2.0 * sx * rng.drand64() + (ox - sx);
    double y = 2.0 * sy * rng.drand64() + (oy - sy);
    double z = 2.0 * sz * rng.drand64() + (oz - sz);
    vgl_point_3d<double> p3d(x, y, z);
    ground_pts.push_back(p3d);
    double u, v;
    rcam.project(x, y, z, u, v);
    image_pts.emplace_back(u, v);
  }
  vpgl_rational_camera<double> fcam;
  bool good = vpgl_rational_camera_compute::compute(image_pts, ground_pts, fcam);
  if (good)
  {
    double rms_er = 0.0;
    for (size_t i = 0; i < n_points; ++i)
    {
      const vgl_point_3d<double> & p3 = ground_pts[i];
      double u, v;
      fcam.project(p3.x(), p3.y(), p3.z(), u, v);
      double sq_err = (u - image_pts[i].x()) * (u - image_pts[i].x()) + (v - image_pts[i].y()) * (v - image_pts[i].y());
      rms_er += sq_err;
    }
    rms_er /= n_points;
    rms_er = sqrt(rms_er);
    std::cout << "fitted rational camera rms projection error " << rms_er << std::endl;
    good = good && (rms_er < 1.0e-5);
  }
  TEST("compute rational camera ", good, true);
}

static void
test_camera_compute()
{
  test_camera_compute_setup();
  test_perspective_compute();
  test_perspective_compute_direct_linear_transform();
  test_perspective_compute_ground();
  test_calibration_compute_natural();
  test_compute_affine();
  test_compute_rational();
}

TESTMAIN(test_camera_compute);
