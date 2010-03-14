#include <testlib/testlib_test.h>

#include <vpgl/vpgl_affine_camera.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
static void test_affine_camera()
{
  //Test construction from ray and point
  vgl_vector_3d<double> ray(-0.577350, -0.577350, -0.577350);
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
  TEST_NEAR("test du, dv", v1+u2+u3,49.5917517+ 49.2928932+ 50.70710678,1e-5); 
  vgl_homg_point_2d<double> p0(u0, v0);
  C.set_viewing_distance(1000);
  vgl_homg_line_3d_2_points<double> l3d = C.backproject(p0);
  vgl_homg_point_3d<double> hpf = l3d.point_finite();
  vgl_point_3d<double> pf(hpf);
  double len = (pf-vgl_point_3d<double>(577.35,577.35,577.35)).length();
  TEST_NEAR("Backproject", len, 0.0, 1.0);
  vgl_homg_plane_3d<double> pp = C.principal_plane();
  double algd = pp.a()*577.350+pp.b()*577.350+pp.c()*577.350+pp.d();
  TEST_NEAR("principal plane", algd, 0.0, 1e-03);
}

TESTMAIN(test_affine_camera);
