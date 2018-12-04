#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_affine_camera.h>
#include <bpgl/algo/bpgl_3d_from_disparity.h>

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

  vil_image_view<float> img3d_truth(2,2,3);
  img3d_truth(0,0,0) = 0.0f; img3d_truth(0,0,1) = 0.0f; img3d_truth(0,0,2) = 4.0;
  img3d_truth(0,1,0) = 0.0f; img3d_truth(0,1,1) = 1.0f; img3d_truth(0,1,2) = 3.0;
  img3d_truth(1,0,0) = 1.0f; img3d_truth(1,0,1) = 0.0f; img3d_truth(1,0,2) = -1.0;
  img3d_truth(1,1,0) = 1.0f; img3d_truth(1,1,1) = 1.0f; img3d_truth(1,1,2) = 5.0;

  vil_image_view<float> disparity(2,2);
  for (int j=0; j<2; ++j) {
    for(int i=0; i<2; ++i) {
      double u1,v1;
      cam1.project(img3d_truth(i,j,0),img3d_truth(i,j,1),img3d_truth(i,j,2),u1,v1);
      double u2,v2;
      cam2.project(img3d_truth(i,j,0),img3d_truth(i,j,1),img3d_truth(i,j,2),u2,v2);
      std::cout << "i,j: " << i << "," << j << "  u1,v1: " << u1 << "," << v1 << " u2,v2: " << u2 << "," << v2 << std::endl;
      disparity(i,j) = u1 - u2;
    }
  }

  vil_image_view<float> img3d_pred = bpgl_3d_from_disparity(cam1, cam2, disparity);

  bool all_good = true;
  for (int j=0; j<2; ++j) {
    for (int i=0; i<2; ++i) {
      std::cout << "i,j: " << i << "," << j << "  truth: " << img3d_truth(i,j,0) << "," << img3d_truth(i,j,1) << "," << img3d_truth(i,j,2);
      std::cout << "   pred: " << img3d_pred(i,j,0) << "," << img3d_pred(i,j,1) << "," << img3d_pred(i,j,2) << std::endl;
      for (int d=0; d<3; ++d) {
        float diff = img3d_pred(i,j,d) - img3d_truth(i,j,d);
        all_good &= std::fabs(diff) < 1e-4;
      }
    }
  }
  TEST("predicted 3D matches truth", all_good, true);
}

static void test_3d_from_disparity()
{
  test_3d_from_disparity_affine();
}

TESTMAIN(test_3d_from_disparity);
