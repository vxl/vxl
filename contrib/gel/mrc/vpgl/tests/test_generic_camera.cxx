#include <testlib/testlib_test.h>

#include <vpgl/vpgl_generic_camera.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
static void test_generic_camera()
{
  //construct a 5x7 image to test pyramid 
  vbl_array_2d<vgl_ray_3d<double> > img(5, 7);
  // simple rays along -z
  vgl_vector_3d<double> dir(0.0, 0.0, -1.0);
  for(int r =0; r<5; ++r)
    for(int c =0; c<7; ++c)
      {
        vgl_point_3d<double> p(c, r, 10.0);// 10 above the ground plane
        img[r][c]=vgl_ray_3d<double>(p, dir);
      }
  vpgl_generic_camera<double> c(img);
#if 0
 
  for(int i = 0; i<2; ++i){
	  vcl_cout << "level " << i << '\n';
		c.print_orig(i);
  }
#endif
  vgl_ray_3d<double> interp_ray = c.ray(3.5, 2.5);
  vgl_point_3d<double> org = interp_ray.origin();
  double er = vcl_fabs(org.x()-3.190983) + vcl_fabs(org.y()-2.190983);
  TEST_NEAR("intepolated ray", er, 0.0, 0.0001);
  double x = 1.5, y = 2.5, z = 0.0, u = 0.0, v= 0.0;
  c.project(x, y, z, u, v);
  er = vcl_fabs(u-1.5) + vcl_fabs(v-2.5);
  x = 3.2, y = 4.1, z = 1.0;
  double u1, v1;
  c.project(x, y, z, u1, v1);
  er += vcl_fabs(u1-3.2) + vcl_fabs(v1-4.1);
  x = 6.8; y = 5.2; z = 0.0;
  double u2, v2;
  c.project(x, y, z, u2, v2);
  er += vcl_fabs(u2-6.8) + vcl_fabs(v2-5.2);
  TEST_NEAR("project", er, 0.0, 0.0001);
}
TESTMAIN(test_generic_camera);
