#include <iostream>
#include "testlib/testlib_test.h"

#include "vpgl/vpgl_proj_camera.h"
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
#include <vgl/algo/vgl_h_matrix_3d.h>
#include "vgl/vgl_ray_3d.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

static void
test_proj_camera()
{
  // Some matrices for testing.
  vnl_matrix_fixed<double, 3, 4> identity_camera(0.0);
  identity_camera(0, 0) = identity_camera(1, 1) = identity_camera(2, 2) = 1;
  double                         random_list[12] = { 1, 15, 9, -1, 2, -6, -9, 7, -5, 6, 10, 0 };
  vnl_matrix_fixed<double, 3, 4> random_matrix(random_list);
  double                         random_list2[12] = { 10.6, 1.009, .676, .5, -13, -10, 8, 5, 88, -2, -100, 11 };
  vnl_matrix_fixed<double, 3, 4> random_matrix2(random_list2);
  double                         random_list3[12] = { 8.6, 66, -.8, 14, -.8, -100, 9.9, 2.4, 7, -1, -18, 90 };
  vnl_matrix_fixed<double, 3, 4> random_matrix3(random_list3);

  // Default constructor.
  vpgl_proj_camera<double> P1;
  TEST("Default constructor", P1.get_matrix(), identity_camera);

  // P1 is of the form [I|0] up to a scale factor
  TEST("P1 is canonical", P1.is_canonical(), true);

  // Construct from array and vnl_fixed_matrix.
  vpgl_proj_camera<double> P2(random_list);
  TEST("Array constructor", P2.get_matrix(), random_matrix);

  vpgl_proj_camera<double> P3(random_matrix2);
  TEST("vnl_fixed_matrix constructor", P3.get_matrix(), random_matrix2);

  // Copy constructor and assignment.
  vpgl_proj_camera<double> P4(P2);
  TEST("Copy constructor", P4.get_matrix(), P2.get_matrix());

  vpgl_proj_camera<double> P5;
  P5 = P2;
  TEST("Assignment operator", P5.get_matrix(), P2.get_matrix());

  P2 = P3;
  TEST("Assignment makes deep copy", P5.get_matrix() != P3.get_matrix(), true);

  // Setters.
  P1.set_matrix(random_matrix);
  TEST("set_matrix from vnl", P1.get_matrix(), random_matrix);
  P2.set_matrix(random_matrix2);
  TEST("set_matrix from array", P2.get_matrix(), random_matrix2);

  // Point projection.
  vgl_homg_point_3d<double>   x1(1, 4, 5);
  vgl_homg_point_2d<double>   y1 = P1.project(x1);
  vnl_vector_fixed<double, 3> y2 = random_matrix * vnl_vector_fixed<double, 4>(1, 4, 5, 1);
  TEST("point projection", y2(0) / y1.x() - y2(1) / y1.y(), 0.0);

  // Line projection.
  vgl_homg_point_3d<double>   x2(4, 3, -10);
  vgl_line_segment_3d<double> l1w(x1, x2);
  vgl_line_segment_2d<double> l1i = P1.project(l1w);
  vgl_line_segment_2d<double> l1ib(P1.project(x1), P1.project(x2));
  TEST_NEAR("line projection", l1i.b() * l1ib.c(), l1ib.b() * l1i.c(), 1e-06);

  // Point backprojection.
  vgl_homg_point_2d<double>         y3(100, 12);
  vgl_homg_line_3d_2_points<double> l3 = P1.backproject(y3);
  vgl_homg_point_2d<double>         y3b = P1.project(l3.point_finite());
  TEST_NEAR("point backprojection", y3.x() * y3b.w(), y3b.x() * y3.w(), 1e-06);
  vgl_ray_3d<double>    r = P1.backproject_ray(y3);
  vgl_point_3d<double>  c(P1.camera_center());
  bool                  org = c == r.origin();
  vgl_vector_3d<double> dirr = r.direction();
  double                dp = dot_product(l3.direction(), dirr);
  TEST("Ray Origin", org, true);
  TEST_NEAR("backprojected ray direction", dp, 1.0, 0.001);
  // Point backprojection - ray direction
  double actual_cam_list[12] = { 437.5,      128.5575,    -153.20889, 20153.20898, 0.0,      -206.5869,
                                 -434.42847, 20434.42968, 0.0,        0.642787,    -0.76604, 100.7660 };
  vnl_matrix_fixed<double, 3, 4> act_cam_matrix(actual_cam_list);
  vpgl_proj_camera<double>       Pact(act_cam_matrix);
  vgl_homg_point_3d<double>      xc = Pact.camera_center();
  double                         u = 0, v = 0;
  Pact.project(0.0, 0.0, 0.0, u, v);
  vgl_homg_point_2d<double>         img_pt(u, v, 1.0);
  vgl_homg_line_3d_2_points<double> line = Pact.backproject(img_pt);
  vgl_homg_point_3d<double>         inf_pt = line.point_infinite();
  vgl_vector_3d<double>             dir(inf_pt.x(), inf_pt.y(), inf_pt.z());
  normalize(dir);
  vgl_vector_3d<double> act_dir(-xc.x(), -xc.y(), -xc.z());
  normalize(act_dir);
  double er = (dir - act_dir).length();
  TEST_NEAR("Ray direction actual", er, 0.0, 1.0e-6);
  // Plane backprojection.
  P2.set_matrix(random_matrix);
  vgl_homg_line_2d<double>  l4(1, -2, 3);
  vgl_homg_plane_3d<double> plane4 = P2.backproject(l4);
  vgl_homg_point_3d<double> q1(1, 0, 0, -plane4.a() / plane4.d());
  vgl_homg_point_3d<double> q2(0, 1, 0, -plane4.b() / plane4.d());
  vgl_homg_point_3d<double> q3(0, 0, 1, -plane4.c() / plane4.d());
  vgl_homg_point_2d<double> q1i = P2.project(q1);
  vgl_homg_point_2d<double> q2i = P2.project(q2);
  vgl_homg_point_2d<double> q3i = P2.project(q3);
  TEST_NEAR("plane backprojection1", vgl_distance(l4, q1i) * vgl_distance(l4, q2i) * vgl_distance(l4, q3i), 0, 1e-06);

  vgl_homg_line_2d<double> l5(-10, 13, 40);
  l5.get_two_points(q1i, q2i);
  vgl_homg_plane_3d<double> plane5a = P2.backproject(l5);
  vgl_homg_plane_3d<double> plane5b(
    P2.backproject(q2i).point_finite(), P2.backproject(q1i).point_infinite(), P2.backproject(q2i).point_infinite());
  TEST_NEAR("plane backprojection2", plane5a.a() * plane5b.d(), plane5b.a() * plane5a.d(), 1e-06);

  // Test automatic SVD computation
  P1.svd();
  P1.set_matrix(random_matrix2);
  TEST_NEAR("automatic svd computation", random_matrix2(2, 3), P1.svd()->recompose()(2, 3), 1e-06);

  // Test get_canonical_h
  vpgl_proj_camera<double>       P6(random_matrix);
  vgl_h_matrix_3d<double>        H = get_canonical_h(P6);
  vnl_matrix_fixed<double, 3, 4> I6 = P6.get_matrix() * H.get_matrix();
  TEST("get_canonical_h",
       std::fabs(I6(0, 0) * I6(1, 1) * I6(2, 2) - 1) < 1e-06 &&
         std::fabs(I6(1, 0) * I6(2, 0) * I6(0, 1) * I6(2, 1) * I6(0, 2) * I6(1, 2) * I6(0, 3) * I6(1, 3) * I6(2, 3)) <
           1e-06,
       true);

  // Test camera center
  vgl_homg_point_2d<double> q6 = P6.project(P6.camera_center());
  TEST_NEAR("camera center computation", q6.x() * q6.y() * q6.w(), 0, 1e-06);

  // Test pre-multiply
  double                         T1array[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  vnl_matrix_fixed<double, 3, 3> T1(T1array);
  P1.set_matrix(random_matrix3);
  P1 = premultiply(P1, T1);
  TEST_NEAR("pre-multiply", P1.get_matrix()(1, 2), (T1 * random_matrix3)(1, 2), 1e-06);

  // Test post-multiply
  double                         T2array[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  vnl_matrix_fixed<double, 4, 4> T2(T2array);
  P1.set_matrix(random_matrix2);
  P1 = postmultiply(P1, T2);
  TEST_NEAR("post-multiply", P1.get_matrix()(1, 0), (random_matrix2 * T2)(1, 0), 1e-06);

  // Test automatic SVD computation
  P1.set_matrix(random_matrix2);
  P1.svd();
  P1.set_matrix(random_matrix3);
  TEST_NEAR("automatic svd computation", random_matrix3(2, 3), P1.svd()->recompose()(2, 3), 1e-06);

  // Test basic projection method
  vpgl_proj_camera<double> Pb;
  Pb.set_matrix(random_matrix);
  u = 0, v = 0;
  double X = x1.x(), Y = x1.y(), Z = x1.z();
  Pb.project(X, Y, Z, u, v);
  TEST_NEAR("base class point projection", y2(0) / u - y2(1) / v, 0.0, 0.001);
  // Test ray intersection
  vnl_matrix_fixed<double, 3, 4> identity, trans;
  identity.set_identity();
  trans.set_identity().put(0, 3, -10);
  vpgl_proj_camera<double> Pi(identity), Pt(trans);
  vgl_point_3d<double>     p3d;
  vgl_point_2d<double>     pi2d(0, 0), pt2d(-1, 0);
  p3d = triangulate_3d_point(Pi, pi2d, Pt, pt2d);
  TEST_NEAR("test triangulate_3d", p3d.z(), 10, 1.0e-6);

  P1.set_matrix(random_matrix);
  std::vector<vgl_point_3d<double>> pts;
  pts.emplace_back(29, -3, 8);
  pts.emplace_back(-0.2, 4.1, 1.0);
  std::vector<vnl_matrix_fixed<double, 2, 3>> Jac = image_jacobians(P1, pts);
  double                                      eps = 1e-6;
  bool                                        valid = true;
  for (unsigned int i = 0; i < pts.size(); ++i)
  {
    vgl_point_3d<double> p = pts[i];
    vgl_point_3d<double> pdx(p.x() + eps, p.y(), p.z());
    vgl_point_3d<double> pdy(p.x(), p.y() + eps, p.z());
    vgl_point_3d<double> pdz(p.x(), p.y(), p.z() + eps);

    vgl_point_2d<double> pi = P1.project(p);
    vgl_point_2d<double> pidx = P1.project(pdx);
    vgl_point_2d<double> pidy = P1.project(pdy);
    vgl_point_2d<double> pidz = P1.project(pdz);

    vnl_matrix_fixed<double, 2, 3> J_diff;
    J_diff(0, 0) = (pidx.x() - pi.x()) / eps;
    J_diff(1, 0) = (pidx.y() - pi.y()) / eps;
    J_diff(0, 1) = (pidy.x() - pi.x()) / eps;
    J_diff(1, 1) = (pidy.y() - pi.y()) / eps;
    J_diff(0, 2) = (pidz.x() - pi.x()) / eps;
    J_diff(1, 2) = (pidz.y() - pi.y()) / eps;

    double err = (J_diff - Jac[i]).array_inf_norm();
    if (err > eps)
    {
      std::cerr << "Jacobian\n" << J_diff << "\nshould be\n" << Jac[i] << std::endl;
      valid = false;
      break;
    }
  }
  TEST("test image Jacobians", valid, true);
}


TESTMAIN(test_proj_camera);
