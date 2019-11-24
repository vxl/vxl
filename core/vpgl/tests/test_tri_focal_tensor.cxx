#include <iostream>
#include "testlib/testlib_test.h"

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vnl/vnl_fwd.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vgl/vgl_point_2d.h"
#include "vgl/vgl_point_3d.h"
#include "vgl/vgl_distance.h"
#include "vgl/vgl_homg_point_2d.h"
#include "vgl/vgl_homg_point_3d.h"
#include "vgl/vgl_homg_line_2d.h"
#include "vgl/vgl_line_segment_2d.h"
#include "vgl/vgl_line_segment_3d.h"
#include "vgl/vgl_homg_line_3d_2_points.h"
#include "vgl/vgl_homg_plane_3d.h"
#include "vgl/vgl_ray_3d.h"
#include <vgl/algo/vgl_h_matrix_3d.h>
#include "vpgl/vpgl_proj_camera.h"
#include "vpgl/vpgl_tri_focal_tensor.h"

static void test_tri_focal_tensor()
{
  vpgl_tri_focal_tensor<double> T0;

  vgl_homg_point_3d<double> X(0.5, 0.5, 2.0, 1.0), X0(1.0, 1.0, 2.0, 1.0);

  // canonical camera
  vpgl_proj_camera<double> c1;
  vgl_homg_point_2d<double> x0(0, 0, 1), x1 = c1.project(X), x10 = c1.project(X0);
  vgl_homg_line_2d<double> l1(x1, x10);
  // move one unit along x
  vnl_matrix_fixed<double,3,4> M( 0.0 );
  M[0][0]=1.0;  M[1][1]=1.0;   M[2][2]=1.0;
  M[0][3] = -1.0;
  vpgl_proj_camera<double> c2(M);
  vgl_homg_point_2d<double> x2 = c2.project(X), x20 = c2.project(X0);

  // move one unit along y
  M[0][3] = 0.0;   M[1][3] = -1.0;
  vpgl_proj_camera<double> c3(M);
  vgl_homg_point_2d<double> x3 = c3.project(X), x30 = c3.project(X0);

  vpgl_tri_focal_tensor<double> T(c1, c2, c3);
  std::cout << T << std::endl;

  vgl_homg_line_2d<double> l2(x2, x20), l3(x3, x30);//(l2 0.5, -0.5, 0.25) (l3 -0.5, 0.5, 0.25)
  vgl_homg_line_2d<double> tl1= T.image1_transfer(l2,l3);
  vgl_homg_line_2d<double> tl2= T.image2_transfer(l1,l3);
  vgl_homg_line_2d<double> tl3 = T.image3_transfer(l1, l2);
  std::cout << "tl1 " << tl1 << std::endl;
  std::cout << "tl2 " << tl2 << std::endl;
  std::cout << "tl3 " << tl2 << std::endl;
  double er_lines = fabs(tl1.a() / tl1.b() +1.0);
  er_lines += fabs(tl2.a() / tl2.c() - 2.0);
  er_lines +=fabs(tl2.b() / tl2.c() + 2.0);
  er_lines += fabs(tl3.a() / tl3.c() + 2.0);
  er_lines +=fabs(tl3.b() / tl3.c() - 2.0);
  TEST_NEAR("line transfer", er_lines, 0.0, 0.0001);
  vgl_homg_point_2d<double> tx1 = T.image1_transfer(x2, x3);
  std::cout << "tx1 " << tx1.x()/tx1.w()<< ' '  << tx1.y()/tx1.w()<< std::endl;
  double er_points = fabs(tx1.x() / tx1.w() - 0.25);
  er_points += fabs(tx1.y() / tx1.w() - 0.25);
  vgl_homg_point_2d<double> tx2 = T.image2_transfer(x1, x3);
  std::cout << "tx2 " << tx2.x()/tx2.w()<< ' '  << tx2.y()/tx2.w()<< std::endl;
  er_points += fabs(tx2.x() / tx2.w() + 0.25);
  er_points += fabs(tx2.y() / tx2.w() - 0.25);
  vgl_homg_point_2d<double> tx3 = T.image3_transfer(x1, x2);
  std::cout << "tx3 " << tx3.x()/tx3.w()<< ' '  << tx3.y()/tx3.w()<< std::endl;
  er_points += fabs(tx3.x() / tx2.w() - 0.25);
  er_points += fabs(tx3.y() / tx3.w() + 0.25);
  TEST_NEAR("point transfer", er_points, 0.0, 0.0001);
  bool good_epipoles = T.compute_epipoles();
  bool good_f_matrices = T.compute_f_matrices();
  bool good_f23 = T.compute_f_matrix_23();
  bool good_p_cams = T.compute_proj_cameras();
  TEST("Good Computations", good_epipoles&&good_f_matrices&&good_f23&&good_p_cams, true);
  vgl_homg_point_2d<double> e12, e13;
  T.get_epipoles(e12, e13);
  std::cout << "e12 " << e12 << " e13 " << e13 << std::endl;
  double er_e12 = fabs(e12.x() / e12.x() - 1.0) + fabs(e12.y()) + fabs(e12.w());
  double er_e13 = fabs(e13.y() / e13.y() - 1.0) + fabs(e13.x()) + fabs(e12.w());
  TEST_NEAR("Epipoles", er_e12 + er_e13, 0.0, 0.0001);
  vpgl_fundamental_matrix<double> f12 = T.fmatrix_12();
  vpgl_fundamental_matrix<double> f13 = T.fmatrix_13();
  vpgl_fundamental_matrix<double> f23 = T.fmatrix_23();
  std::cout << "F12\n" << f12 << std::endl;
  std::cout << "F13\n" << f13 << std::endl;
  std::cout << "F23\n" << f23 << std::endl;
  vgl_homg_line_2d<double> epl2 = f12.r_epipolar_line(x1);
  double alg_dist_2 = x2.x()*epl2.a() + x2.y()*epl2.b() + x2.w()*epl2.c();
  TEST_NEAR("Fundamental Matrix F12", fabs(alg_dist_2), 0.0, 0.0001);
  vgl_homg_line_2d<double> epl3 = f13.r_epipolar_line(x1);
  double alg_dist_3 = x3.x()*epl3.a() + x3.y()*epl3.b() + x3.w()*epl3.c();
  TEST_NEAR("Fundamental Matrix F13", fabs(alg_dist_3), 0.0, 0.0001);
  vgl_homg_line_2d<double> epl23 = f23.r_epipolar_line(x2);
  double alg_dist_23 = x3.x()*epl23.a() + x3.y()*epl23.b() + x3.w()*epl23.c();
  TEST_NEAR("Fundamental Matrix F23", fabs(alg_dist_23), 0.0, 0.0001);

  vgl_h_matrix_2d<double> H13 = T.hmatrix_13(epl3);
  vgl_h_matrix_2d<double> H12 = T.hmatrix_12(epl2);
  std::cout << "H13\n" << H13 << std::endl;
  std::cout << "H12\n" << H12 << std::endl;
  std::cout << "x13 " << H13*x3 << std::endl;
  std::cout << "x12 " << H12*x2 << std::endl;

  // matrix three point constraint
  vnl_matrix_fixed<double,3,3> zero_3x3 = T.point_constraint_3x3(x1,x2,x3);
  std::cout << "zero 3x3\n" << zero_3x3 << std::endl;
  double fbz = zero_3x3.frobenius_norm();
  TEST_NEAR("perfect tri point match", fbz, 0.0, 0.000001);

  //scalar three point constraint
  double scalar_zero = T.point_constraint(x1, x2, x3);
  std::cout << "scalar zero = " << scalar_zero << std::endl;
  TEST_NEAR("perfect scalar tri point match", scalar_zero, 0.0, 0.000001);

  // test with perturbed points
  double d = 1.5;
  vgl_homg_point_2d<double> x2p((x2.x()/x2.w())+d, (x2.y()/x2.w())+d);
  vnl_matrix_fixed<double,3,3> zero_3x3_p = T.point_constraint_3x3(x1,x2p,x3);
  std::cout << "zero 3x3 perturbed\n" << zero_3x3_p << std::endl;
  double fbp = zero_3x3_p.frobenius_norm();
  TEST("perturbed tri point match", fbp>0.0, true);
  // scalar point constraint
  double scalar_zero_p = T.point_constraint(x1, x2p, x3);
  std::cout << "perturbed scalar zero = " << scalar_zero_p << std::endl;
  TEST("perturbed scalar tri point match", scalar_zero_p>0.0, true);
  // test homographies

  // from a line in image 3
  vgl_homg_line_2d<double> line_3(x30, x3);
  line_3.normalize();

  vgl_h_matrix_2d<double> HL12 = T.hmatrix_12(line_3);
  vgl_homg_point_2d<double> p2_HL12 = HL12*x1;
  vgl_point_2d<double> p2(p2_HL12), p2_gt(x2);
  std::cout << "p2 " << p2 << " vs. p2_gt " << p2_gt<< std::endl;
  double er_p2 = (p2 - p2_gt).length();
  TEST_NEAR("H12 from line", er_p2, 0.0, 0.001);

  // from a line in image 2
  vgl_homg_line_2d<double> line_2(x20, x2);
  line_2.normalize();

  vgl_h_matrix_2d<double> HL13 = T.hmatrix_13(line_2);
  vgl_homg_point_2d<double> p3_HL13 = HL13*x1;
  vgl_point_2d<double> p3(p3_HL13), p3_gt(x3);
  std::cout << "p3 " << p3 << " vs. p3_gt " << p3_gt<< std::endl;
  double er_p3 = (p3 - p3_gt).length();
  TEST_NEAR("H13 from line", er_p3, 0.0, 0.001);
  //  test line constraint
  double one_sixth = 1.0 / 6.0;
  vgl_homg_point_2d<double> hlc10(0.25, 0.25), hlc11(one_sixth, one_sixth);
  vgl_homg_point_2d<double> hlc20(-0.25, 0.25), hlc21(-0.5, one_sixth);
  vgl_homg_point_2d<double> hlc30(0.25, -0.25), hlc31(one_sixth, -0.5);
  vgl_homg_line_2d<double> lc1(hlc10, hlc11),lc2(hlc20, hlc21),lc3(hlc30, hlc31);
  vnl_vector_fixed<double, 3> lc_3 = T.line_constraint_3(lc1, lc2, lc3);
  std::cout << "line constraint 3 " << lc_3 << std::endl;
  double er_lc = lc_3.magnitude();
  TEST_NEAR("three line constraint", er_lc, 0.0, 1e-10);
}


TESTMAIN(test_tri_focal_tensor);
