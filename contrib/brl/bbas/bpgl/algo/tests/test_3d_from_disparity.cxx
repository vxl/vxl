#include <iostream>
#include <vector>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vil/vil_image_view.h"
#include "vnl/vnl_math.h"
#include "vgl/vgl_point_2d.h"
#include "vpgl/vpgl_affine_camera.h"
#include "vpgl/vpgl_perspective_camera.h"
#include <bpgl/algo/bpgl_3d_from_disparity.h>


template<typename T>
static void test_3d_from_disparity_affine()
{
  double theta = vnl_math::pi_over_180 * 45.0;
  double z = 5.0;

  // camera 1 is orthographic identity: u,v = x,y
  vnl_matrix_fixed<double,3,4> P1(0.0);
  P1[0][0] = 1; P1[0][1] = 0; P1[0][2] = 0; P1[0][3] = 0;
  P1[1][0] = 0; P1[1][1] = 1; P1[1][2] = 0; P1[1][3] = 0;
  P1[2][3] = 1;
  vpgl_affine_camera<double> cam1(P1);

  // camera 2 u,v = f(x,z),v
  vnl_matrix_fixed<double,3,4> P2(0.0);
  P2[0][0] = std::cos(theta); P2[0][1] = 0; P2[0][2] = std::sin(theta); P2[0][3] = 0;
  P2[1][0] = 0; P2[1][1] = 1; P2[1][2] = 0; P2[1][3] = 0;
  P2[2][3] = 1;
  vpgl_affine_camera<double> cam2(P2);

  vil_image_view<T> img3d_truth(2,2,3);
  img3d_truth(0,0,0) = T(0.0f); img3d_truth(0,0,1) = T(0.0f); img3d_truth(0,0,2) = T(4.0);
  img3d_truth(0,1,0) = T(0.0f); img3d_truth(0,1,1) = T(1.0f); img3d_truth(0,1,2) = T(3.0);
  img3d_truth(1,0,0) = T(1.0f); img3d_truth(1,0,1) = T(0.0f); img3d_truth(1,0,2) = T(-1.0);
  img3d_truth(1,1,0) = T(1.0f); img3d_truth(1,1,1) = T(1.0f); img3d_truth(1,1,2) = T(5.0);

  vil_image_view<T> disparity(2,2);
  for (int j=0; j<2; ++j) {
    for(int i=0; i<2; ++i) {
      double u1,v1;
      cam1.project(img3d_truth(i,j,0),img3d_truth(i,j,1),img3d_truth(i,j,2),u1,v1);
      double u2,v2;
      cam2.project(img3d_truth(i,j,0),img3d_truth(i,j,1),img3d_truth(i,j,2),u2,v2);
      std::cout << "i,j: " << i << "," << j << "  u1,v1: " << u1 << "," << v1 << " u2,v2: " << u2 << "," << v2 << std::endl;
      disparity(i,j) = T(u2 - u1); // jlm changed to be consistent with bsgm_disparity_estimator
    }
  }

  vil_image_view<T> img3d_pred = bpgl_3d_from_disparity<T, vpgl_affine_camera<double> >(cam1, cam2, disparity);

  bool all_good = true;
  for (int j=0; j<2; ++j) {
    for (int i=0; i<2; ++i) {
      std::cout << "i,j: " << i << "," << j << "  truth: " << img3d_truth(i,j,0) << "," << img3d_truth(i,j,1) << "," << img3d_truth(i,j,2);
      std::cout << "   pred: " << img3d_pred(i,j,0) << "," << img3d_pred(i,j,1) << "," << img3d_pred(i,j,2) << std::endl;
      for (int d=0; d<3; ++d) {
        double diff = img3d_pred(i,j,d) - img3d_truth(i,j,d);
        all_good &= std::fabs(diff) < 1e-4;
      }
    }
  }

  TEST("predicted 3D matches truth", all_good, true);
}
template <class T>
static void test_3d_from_disparity_perspective() {
  double zavg = 10000;
  size_t ni = 100, nj = 100;
  //camera 0
  double kd[9] = { 8829.15, 0, 50,
    0, 8829.15, 50,
    0, 0, 1 };
  vnl_matrix_fixed<double, 3, 3> Km(kd);
  vpgl_calibration_matrix<double> K(Km);

  vnl_matrix_fixed<double, 3, 3> Rm;
  Rm.set_identity();
  vgl_rotation_3d<double> R0(Rm);
  vgl_vector_3d<double> t0(0.0, 0.0, 0.0);
  vpgl_perspective_camera<double> P0(K, R0, t0);
  //camera 1
  vgl_vector_3d<double> t1(10.0, 0.0, 0.0);
  vpgl_perspective_camera<double> P1(K, R0, t1);
  vil_image_view<T> disparity(ni, nj);
  disparity.fill(0.0f);
  std::vector<vgl_point_3d<T> > pts_3d;
  std::vector<std::pair<size_t, size_t> > img_pts;
  
  double r = 10;//range in xy
  for (double x = -r; x <= r; x += 1)
    for (double y = -r; y <= r; y += 1) {
      double z = zavg + x * y;
      pts_3d.emplace_back(x, y, z);
      double u00, v00, u10, v10;
      P0.project(x,y,z, u00, v00);
      P1.project(x, y, z, u10, v10);
      size_t ut = std::round(u00), vt = std::round(v00);
      img_pts.emplace_back(ut, vt);
      T d = T(u10 - u00);
      disparity(ut, vt) = d;
    }
  vil_image_view<T> img3d_pred = bpgl_3d_from_disparity<T, vpgl_perspective_camera<double> >(P0, P1, disparity);
  size_t n = pts_3d.size();
  T dsum = T(0);
  for (size_t i = 0; i < n; ++i) {
    size_t u = img_pts[i].first, v = img_pts[i].second;
    T disp = disparity(u, v);
    T x = img3d_pred(u, v, 0);
    T y = img3d_pred(u, v, 1);
    T z = img3d_pred(u, v, 2);
    //round the reference location to integer pixels
    double ix = std::round(x), iy = std::round(y);
    vgl_point_3d<T> p(ix, iy, z), ptruth = pts_3d[i];
    vgl_vector_3d<T> er = p - ptruth;
    T dist = er.length()/zavg;
    dsum += dist;
  }
  dsum /= n;
  TEST_NEAR("distance to ground truth 3d pts", dsum, T(0), 2e-4);
}
static void test_3d_from_disparity()
{
  std::cout << "bpgl_3d_from_disparity affine for template<float>" << std::endl;
  test_3d_from_disparity_affine<float>();
  std::cout << "bpgl_3d_from_disparity affine for template<double>" << std::endl;
  test_3d_from_disparity_affine<double>();
  std::cout << "bpgl_3d_from_disparity perspective for template<double>" << std::endl;
  test_3d_from_disparity_perspective<double>();
  std::cout << "bpgl_3d_from_disparity perspective for template<float>" << std::endl;
  test_3d_from_disparity_perspective<float>();
}

TESTMAIN(test_3d_from_disparity);
