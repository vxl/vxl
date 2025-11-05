#include "testlib/testlib_test.h"

#include "vpgl/vpgl_affine_camera.h"
#include "vgl/vgl_vector_3d.h"
#include "vgl/vgl_point_3d.h"
#include "vgl/vgl_ray_3d.h"
#include "vgl/vgl_distance.h"
#include "vnl/vnl_math.h"


static void
test_affine_camera()
{
  // Test construction from ray and point
  const double sq2 = vnl_math::sqrt1_2;
  const double sq3 = vnl_math::sqrt1_3;
  const vgl_vector_3d<double> ray(-sq3, -sq3, -sq3);
  const vgl_vector_3d<double> up(0, 0, 1);
  vgl_point_3d<double> p(0.5, 0.5, 0.0);
  double u0 = 50.0, v0 = 50.0;
  double du = 1.0, dv = 1.0;
  vpgl_affine_camera<double> C(ray, up, p, u0, v0, du, dv);
  double u00 = 0, v00 = 0;
  double u1 = 0, v1 = 0, u2 = 0, v2 = 0, u3 = 0, v3 = 0;
  double x1 = p.x() - 0.5, y1 = p.y() - 0.5, z1 = 0;
  double x2 = x1 + 1.0, y2 = y1, z2 = z1;
  double x3 = x1, y3 = y1 + 1, z3 = z1;
  C.project(p.x(), p.y(), p.z(), u00, v00);
  TEST_NEAR("test center projection", u00 + v00, 100, 1e-5);
  C.project(x1, y1, z1, u1, v1);
  C.project(x2, y2, z2, u2, v2);
  C.project(x3, y3, z3, u3, v3);
  TEST_NEAR("test du, dv", v1 + u2 + u3, 49.591751709536 + 50 - sq2 + 50 + sq2, 1e-9);
  const vgl_homg_point_2d<double> p0(u0, v0);
  C.set_viewing_distance(1000);
  const vgl_homg_point_3d<double> cam_center = C.camera_center();
  const vgl_vector_3d<double> cc_dir(cam_center.x(), cam_center.y(), cam_center.z());
  double len = (ray - cc_dir).length();
  TEST_NEAR("Camera center", len, 0.0, 1e-8);
  const vgl_homg_line_3d_2_points<double> l3d = C.backproject(p0);
  const vgl_homg_point_3d<double> hpf = l3d.point_finite();
  const vgl_point_3d<double> pf(hpf);
  len = (pf - vgl_point_3d<double>(1000 * sq3, 1000 * sq3, 1000 * sq3)).length();
  TEST_NEAR("Backproject", len, 0.0, 1.0);
  const vgl_homg_plane_3d<double> pp = C.principal_plane();
  const double algd = (pp.a() + pp.b() + pp.c()) * 1000 * sq3 + pp.d();
  TEST_NEAR("principal plane", algd, 0.0, 1e-08);

  // test realistic affine camera
  vnl_vector_fixed<double, 4> row1, row2;
  row1[0] = 1.3483235713495938;
  row1[1] = 0.0038174980872772743;
  row1[2] = 0.27647870881886161;
  row1[3] = 8.8599950663932052;

  row2[0] = 0.21806927892797245;
  row2[1] = -0.92631091145800215;
  row2[2] = -1.0010535330976205;
  row2[3] = 538.93376518200034;

  vpgl_affine_camera<double> row_cam(row1, row2);
  row_cam.set_viewing_distance(9325.6025071654913);
  const vgl_vector_3d<double> row_ray_dir = row_cam.ray_dir();
  const vgl_vector_3d<double> test_dir(0.13271023792536230, 0.74172901339587183, -0.65743901879686173);
  TEST_NEAR("row ray dir", (row_ray_dir - test_dir).length(), 0.0, 1e-5);

  const vgl_point_3d<double> wrld_pt(274.30804459155252, 85.614875071463018, -35.273309156122778);
  const vgl_homg_point_2d<double> img_pt(369.28342202880049, 554.72813713086771);
  const vgl_ray_3d<double> row_ray = row_cam.backproject_ray(img_pt);
  const double ray_dist = vgl_distance(row_ray, wrld_pt);
  TEST_NEAR("row_ray_dist", ray_dist, 0.0, 0.05);

  // test postmultiply with a translation
  const vgl_vector_3d<double> trans(3.0, -10.0, 5.0);
  const vpgl_affine_camera<double> trans_row_cam = postmultiply_a(row_cam, trans);
  const vgl_homg_point_2d<double> hpt = trans_row_cam.project(vgl_point_3d<double>(0, 0, 0));
  const vgl_point_2d<double> pt(hpt);
  const vgl_point_2d<double> test_pt(14.24918434, 543.8458145);
  const double pdist = (pt - test_pt).length();
  TEST_NEAR("postmultiply translation", pdist, 0.0, 0.0001);
}

TESTMAIN(test_affine_camera);
