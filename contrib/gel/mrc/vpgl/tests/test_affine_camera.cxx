#include <testlib/testlib_test.h>

#include <vpgl/vpgl_affine_camera.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_math.h>

static void test_affine_camera()
{
  //Test construction from ray and point
  const double sq2 = vnl_math::sqrt1_2;
  const double sq3 = vnl_math::sqrt1_3;
  vgl_vector_3d<double> ray(-sq3, -sq3, -sq3);
  vgl_vector_3d<double> up(0,0,1);
  vgl_point_3d<double> p(0.5, 0.5, 0.0);
  double u0 = 50.0, v0 = 50.0;
  double du = 1.0, dv = 1.0;
  vpgl_affine_camera<double> C(ray, up, p, u0, v0, du, dv);
  double u00= 0, v00= 0;
  double u1 = 0, v1 = 0, u2 = 0, v2 = 0, u3 = 0, v3 = 0;
  double x1 = p.x()-0.5, y1 = p.y()-0.5, z1 = 0;
  double x2 = x1+1.0, y2 = y1, z2 = z1;
  double x3 = x1, y3 = y1+1, z3 = z1;
  C.project(p.x(), p.y(), p.z(), u00, v00);
  TEST_NEAR("test center projection", u00+v00, 100, 1e-5);
  C.project(x1, y1, z1, u1, v1);
  C.project(x2, y2, z2, u2, v2);
  C.project(x3, y3, z3, u3, v3);
  TEST_NEAR("test du, dv", v1+u2+u3, 49.591751709536 + 50-sq2 + 50+sq2, 1e-9);
  vgl_homg_point_2d<double> p0(u0, v0);
  C.set_viewing_distance(1000);
  vgl_homg_point_3d<double> cam_center = C.camera_center();
  vgl_vector_3d<double> cc_dir(cam_center.x(), cam_center.y(), cam_center.z());
  double len = (ray-cc_dir).length();
  TEST_NEAR("Camera center", len, 0.0, 1e-8);
  vgl_homg_line_3d_2_points<double> l3d = C.backproject(p0);
  vgl_homg_point_3d<double> hpf = l3d.point_finite();
  vgl_point_3d<double> pf(hpf);
  len = (pf-vgl_point_3d<double>(1000*sq3,1000*sq3,1000*sq3)).length();
  TEST_NEAR("Backproject", len, 0.0, 1.0);
  vgl_homg_plane_3d<double> pp = C.principal_plane();
  double algd = (pp.a()+pp.b()+pp.c())*1000*sq3+pp.d();
  TEST_NEAR("principal plane", algd, 0.0, 1e-08);
}

TESTMAIN(test_affine_camera);
