#include <limits>
#include <iostream>
#include <testlib/testlib_test.h>
#include <vpgl/algo/vpgl_affine_rectification.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_affine_rect()
{
  vnl_matrix_fixed<double,3,4> M1;
  M1[0][0] = 2.50119; M1[0][1] = -0.0254316; M1[0][2] = -0.289286; M1[0][3] = 75.9139;
  M1[1][0] = -0.0249724; M1[1][1] = -2.50137; M1[1][2] = 1.25655; M1[1][3] = 2546.31;
  M1[2][0] = 0; M1[2][1] = 0; M1[2][2] = 0; M1[2][3] = 1;

  vpgl_affine_camera<double> cam1(M1);

  vnl_matrix_fixed<double,3,4> M2;
  M2[0][0] = -0.00389854; M2[0][1] = -2.44708; M2[0][2] = -0.0605; M2[0][3] = 2471.27;
  M2[1][0] = -2.43285; M2[1][1] = 0.0128908; M2[1][2] = -0.270819; M2[1][3] = 2481.26;
  M2[2][0] = 0; M2[2][1] = 0; M2[2][2] = 0; M2[2][3] = 1;

  vpgl_affine_camera<double> cam2(M2);

  vpgl_affine_fundamental_matrix<double> FA;
  TEST("compute affine f", vpgl_affine_rectification::compute_affine_f(&cam1, &cam2, FA), true);
  //TEST_NEAR("Solution Correct (fixed K)", rms_3d_pts, 0.0, 2.0e-3);

  // use an image correspondence to check the fundamental matrix
  vnl_matrix_fixed<double, 3, 1> P1;
  P1[0][0] = 1295.131348; P1[1][0] = 1364.181763; P1[2][0] = 1;

  vnl_matrix_fixed<double, 3, 1> P2;
  P2[0][0] = 1241.862915; P2[1][0] = 1253.801758; P2[2][0] = 1;

  vnl_matrix_fixed<double, 3, 3> FAM = FA.get_matrix();
  vgl_homg_point_2d<double> er, el;
  FA.get_epipoles(er, el);
  std::cout << "er: " << er << " el: " << el << std::endl;

  vnl_matrix_fixed<double, 1, 1> val = P2.transpose()*FAM*P1;
  std::cout << val[0][0] << std::endl;
  TEST("check computed f", std::abs(val[0][0]) < 0.01, true);

  // test computation of rectification matrices
  std::vector<vnl_vector_fixed<double, 3> > img_p1, img_p2;
  vnl_vector_fixed<double, 3> p1;
  p1[0] = P1[0][0]; p1[1] = P1[1][0]; p1[2] = P1[2][0];
  vnl_vector_fixed<double, 3> p2;
  p2[0] = P2[0][0]; p2[1] = P2[1][0]; p2[2] = P2[2][0];
  img_p1.push_back(p1); img_p2.push_back(p2);
  vnl_matrix_fixed<double, 3, 3> H1, H2;
  TEST("compute rectification", vpgl_affine_rectification::compute_rectification(FA, img_p1, img_p2, H1, H2), true);

  std::cout << "\nH1:\n" << H1 << std::endl;
  std::cout << "H2:\n" << H2 << std::endl;

}

TESTMAIN(test_affine_rect);
