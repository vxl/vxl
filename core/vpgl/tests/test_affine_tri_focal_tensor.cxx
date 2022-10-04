#include <iostream>
#include <sstream>
#include "testlib/testlib_test.h"

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vnl/vnl_fwd.h"
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
#include "vpgl/vpgl_affine_tri_focal_tensor.h"
#include "vpgl/vpgl_affine_camera.h"
#include "vpgl/vpgl_proj_camera.h"
#include "vpgl/vpgl_fundamental_matrix.h"
#include "vpgl/vpgl_affine_fundamental_matrix.h"

static void
test_affine_tri_focal_tensor()
{
  vpgl_affine_camera<double> c1; // identity camera

  double temp = 1.0 / sqrt(2.0);
  vgl_point_3d<double> sp(1.0, 1.0, 2.0);
  vgl_vector_3d<double> ray2(-temp, 0.0, temp), ray3(0.0, -temp, temp);
  vgl_vector_3d<double> up23(0.0, -1.0, 0.0);
  double u2 = -1.0, v2 = 1.0, u3 = 1.0, v3 = -1.0, su = 1.0, sv = 1.0;
  vgl_homg_point_2d<double> x1(1, 1), x2(-1, 1), x3(1, -1);
  vpgl_affine_camera<double> c2(ray2, up23, sp, u2, v2, su, sv);
  vpgl_affine_camera<double> c3(ray3, up23, sp, u3, v3, su, sv);
  std::cout << "c1\n" << c1 << std::endl;
  std::cout << "c2\n" << c2 << std::endl;
  std::cout << "c3\n" << c3 << std::endl;
  vpgl_affine_tri_focal_tensor<double> aT(c1, c2, c3);
  bool good = aT.compute();
  TEST("Good Computations", good, true);
  std::cout << aT << std::endl;
  vpgl_affine_fundamental_matrix<double> aF12 = aT.affine_fmatrix_12();
  vpgl_affine_fundamental_matrix<double> aF13 = aT.affine_fmatrix_13();
  vpgl_affine_fundamental_matrix<double> aF23 = aT.affine_fmatrix_23();
  std::cout << "aF12 \n" << aF12 << std::endl;
  std::cout << "aF13 \n" << aF13 << std::endl;
  std::cout << "aF23 \n" << aF23 << std::endl;
  vnl_matrix_fixed<double, 3, 3> m12 = aF12.get_matrix();
  double f12_ratio = m12[1][2] / m12[2][1];
  double er = (f12_ratio + 1.0);
  vnl_matrix_fixed<double, 3, 3> m13 = aF13.get_matrix();
  double f13_ratio = m13[0][2] / m13[2][0];
  er += (f13_ratio + 1);
  vnl_matrix_fixed<double, 3, 3> m23 = aF23.get_matrix(); // delay test of f23

  TEST_NEAR("Affine fundamental matrices", er, 0.0, 0.001);
  vpgl_affine_camera<double> ac1 = aT.affine_camera_1();
  vpgl_affine_camera<double> ac2 = aT.affine_camera_2();
  vpgl_affine_camera<double> ac3 = aT.affine_camera_3();
  std::cout << "ac1 \n" << ac1 << std::endl;
  std::cout << "ac2 \n" << ac2 << std::endl;
  std::cout << "ac3 \n" << ac3 << std::endl;
  vgl_homg_line_2d<double> line2(vgl_homg_point_2d<double>(u2, v2), vgl_homg_point_2d<double>(u2 - 1, v2 + 1));

  vgl_homg_point_2d<double> tx1 = aT.image1_transfer(x2, x3);
  er = fabs(tx1.x() / tx1.w() - 1.0) + fabs(tx1.y() / tx1.w() - 1.0);
  std::cout << "tx1 " << tx1 << std::endl;
  vgl_homg_point_2d<double> tx2 = aT.image2_transfer(x1, x3);
  er += fabs(tx2.x() / tx2.w() + 1.0) + fabs(tx2.y() / tx2.w() - 1.0);
  std::cout << "tx2 " << tx2 << std::endl;
  vgl_homg_point_2d<double> tx3 = aT.image3_transfer(x1, x2);
  er += fabs(tx3.x() / tx3.w() - 1.0) + fabs(tx3.y() / tx3.w() + 1.0);
  std::cout << "tx3 " << tx3 << std::endl;
  TEST_NEAR("point transfer", er, 0.0, 0.0001);

  vnl_matrix_fixed<double, 3, 3> zero_3x3 = aT.point_constraint_3x3(x1, x2, x3);
  std::cout << "zero 3x3\n" << zero_3x3 << std::endl;
  double fbz = zero_3x3.frobenius_norm();
  TEST_NEAR("perfect tri point match", fbz, 0.0, 0.000001);

  // move x2
  double d = 1.5;
  vgl_homg_point_2d<double> x2p((x2.x() / x2.w()) + d, (x2.y() / x2.w()) + d);
  vnl_matrix_fixed<double, 3, 3> zero_3x3_p = aT.point_constraint_3x3(x1, x2p, x3);
  std::cout << "zero 3x3_p\n" << zero_3x3_p << std::endl;
  double fbp = zero_3x3_p.frobenius_norm();
  TEST("perturbed tri point match", fbp > 0.0, true);
  // scalar point constraint
  double scalar_zero = aT.point_constraint(x1, x2, x3);
  std::cout << "scalar zero = " << scalar_zero << std::endl;
  TEST_NEAR("perfect scalar tri point match", scalar_zero, 0.0, 0.000001);
  double scalar_zero_p = aT.point_constraint(x1, x2p, x3);
  std::cout << "perturbed scalar zero = " << scalar_zero_p << std::endl;
  TEST("perturbed scalar tri point match", fabs(scalar_zero_p) > 0.0, true);
  vgl_point_3d<double> p3d2(2.0, 2.0, 2.0);
  vgl_homg_point_2d<double> pc1 = c1.project(p3d2);
  vgl_homg_point_2d<double> pc2 = c2.project(p3d2);
  vgl_homg_point_2d<double> pc3 = c3.project(p3d2);
  vgl_homg_line_2d<double> lc1(x1, pc1), lc2(x2, pc2), lc3(x3, pc3);
  vnl_vector_fixed<double, 3> lc_3 = aT.line_constraint_3(lc1, lc2, lc3);
  std::cout << "line constraint 3 " << lc_3 << std::endl;
  double er_lc = lc_3.magnitude();
  TEST_NEAR("three line constraint", er_lc, 0.0, 1e-10);
}


TESTMAIN(test_affine_tri_focal_tensor);
