#include <iostream>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


#include <vpgl/algo/vpgl_affine_fm_compute_5_point.h>
#include "vpgl/vpgl_affine_fundamental_matrix.h"
#include "vpgl/vpgl_affine_camera.h"
#include "vnl/vnl_fwd.h"
#include "vnl/vnl_double_3x3.h"
#include "vgl/vgl_homg_point_2d.h"
#include "vgl/vgl_homg_point_3d.h"
#include <vnl/vnl_random.h>
static void
test_affine_fm_compute()
{
  // realistic affine cameras

  std::vector<std::vector<double>> L = {
    { 1.4008339964662238, 0.01379357608876183, 0.29176389136913616, 168.94607022009185 },
    { 0.21473016282540966, -1.0077720142913837, -1.01170300092531, 848.158895152373 },
    { 0.0, 0.0, 0.0, 1.0 }
  };


  std::vector<std::vector<double>> R = {
    { 2.089284265352598, -0.0004077472495484305, -0.32278114834646787, 357.20654992768976 },
    { -0.04702210619889386, -2.1029195387187127, -0.29754243403187125, 917.4450068245351 },
    { 0.0, 0.0, 0.0, 1.0 }
  };
  vnl_matrix_fixed<double, 3, 4> m_r, m_l; // target, source camera matrices
  for (size_t row = 0; row < 3; ++row)
    for (size_t col = 0; col < 4; ++col)
    {
      m_r[row][col] = R[row][col];
      m_l[row][col] = L[row][col];
    }
  vpgl_affine_camera<double> cam_R(m_r), cam_L(m_l);
  // reference fundamental matrix
  vpgl_affine_fundamental_matrix<double> F(cam_R, cam_L); // fundamental matrix xl^T F xr
  vnl_matrix_fixed<double, 3, 3> mf = F.get_matrix();
  // zero out upper 2x2
  mf[0][0] = 0.0;
  mf[0][1] = 0.0;
  mf[1][0] = 0.0;
  mf[1][1] = 0.0;
  double a = mf[0][2], b = mf[1][2], s = sqrt(a * a + b * b);
  mf /= s;
  if (mf[2][2] < 0.0)
    mf *= -1.0;
  std::cout << "True F \n" << mf << std::endl;

  // generate perfect correspondences
  vnl_random rand;
  std::vector<vgl_point_2d<double>> r_pts, l_pts;
  double x_min = 0.0, x_max = 500.0, inc = 100.0;
  double y_min = 0.0, y_max = 400.0;
  double z_min = 0.0, z_max = 300.0;
  for (size_t i = 0; i < 100; ++i)
  {
    double x = rand.drand32(x_min, x_max);
    double y = rand.drand32(y_min, y_max);
    double z = rand.drand32(z_min, z_max);
    vgl_point_3d<double> p3d(x, y, z);
    vgl_point_2d<double> p2d_r = cam_R.project(p3d);
    vgl_point_2d<double> p2d_l = cam_L.project(p3d);
    r_pts.push_back(p2d_r);
    l_pts.push_back(p2d_l);
  }
  vpgl_affine_fundamental_matrix<double> fm;
  //                         normalize pts verbose
  vpgl_affine_fm_compute_5_point fc(true, true);
  fc.compute(r_pts, l_pts, fm);
  vnl_matrix_fixed<double, 3, 3> fmm = fm.get_matrix();
  if (fmm[2][2] < 0.0)
    fmm *= -1.0;
  std::cout << "Test F \n" << fmm << std::endl;
  TEST_NEAR("affine fm robust ransac from perfect correspondences", (fmm - mf).frobenius_norm(), 0, 1);
  std::vector<vgl_point_2d<double>> min_r_pts, min_l_pts;
  for (size_t i = 0; i < 5; ++i)
  {
    min_r_pts.push_back(r_pts[i]);
    min_l_pts.push_back(l_pts[i]);
  }
  vpgl_affine_fm_compute_5_point min_fc(true, true);
  min_fc.compute(min_r_pts, min_l_pts, fm);
  fmm = fm.get_matrix();
  if (fmm[2][2] < 0.0)
    fmm *= -1.0;
  std::cout << "Test F \n" << fm.get_matrix() << std::endl;
  TEST_NEAR("affine fm robust ransac from min number of correspondences", (fmm - mf).frobenius_norm(), 0, 1);
}

TESTMAIN(test_affine_fm_compute);
