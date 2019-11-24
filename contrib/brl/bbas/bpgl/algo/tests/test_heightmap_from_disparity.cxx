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
#include <bpgl/algo/bpgl_heightmap_from_disparity.h>


template<typename T>
static void test_heightmap_from_disparity_affine()
{
  double theta = vnl_math::pi_over_180 * 45.0;
  double z = 5.0;
  T eps = 1e-6;

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

  // convert img3d into heightmap
  vil_image_view<T> hmap_truth(2,2);
  for (int j=0; j<2; ++j) {
    for (int i=0; i<2; ++i) {
      hmap_truth(i,j) = img3d_truth(i,1-j,2);
    }
  }

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

  vgl_box_3d<T> hmap_bounds(vgl_point_3d<T>(0,0,-2), vgl_point_3d<T>(1,1,6));
  T GSD = 1.0;
  vil_image_view<T> hmap_pred = bpgl_heightmap_from_disparity(cam1, cam2, disparity,
                                                              hmap_bounds, GSD);

  TEST_EQUAL("heightmap size (x)", hmap_pred.ni(), 2);
  TEST_EQUAL("heightmap size (y)", hmap_pred.nj(), 2);

  bool all_good = true;
  for (int j=0; j<2; ++j) {
    for (int i=0; i<2; ++i) {
      std::cout << "i,j: " << i << "," << j << "  truth: " << hmap_truth(i,j) << "   pred: " << hmap_pred(i,j) << std::endl;
      double diff = hmap_pred(i,j) - hmap_truth(i,j);
      all_good &= std::fabs(diff) < 1e-2; // i.e., accurate to a cm (0.01 meters)
    }
  }
  TEST("predicted heights match truth", all_good, true);
}

static void test_heightmap_from_disparity()
{
  std::cout << "bpgl_heightmap_from_disparity for template<float>" << std::endl;
  test_heightmap_from_disparity_affine<float>();
  std::cout << "bpgl_heightmap_from_disparity for template<double>" << std::endl;
  test_heightmap_from_disparity_affine<double>();
}

TESTMAIN(test_heightmap_from_disparity);
