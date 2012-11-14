#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <bbas/volm/volm_spherical_shell_container.h>
#include <bbas/volm/volm_spherical_shell_container_sptr.h>

static void test_spherical_shell_container()
{
  float cap_angle = 180; // = 90;
  float point_angle = 10; // = 30;
  double radius = 1;
  float top_angle = 40;
  float bottom_angle = 20;
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(radius, cap_angle, point_angle, top_angle, bottom_angle);

  vcl_cout << " for half spherical surface, point angle = " << point_angle << " degree, number of rays: " << sph_shell->get_container_size() << '\n'
           << " radius = " << sph_shell->radius() << " ,  cap_angle = " << sph_shell->cap_angle() << ", point_angle = " << sph_shell->point_angle()
           << " spherical center = " << sph_shell->cent() << '\n'
           << " for half spherical surface, point angle = " << point_angle << " degree, number of rays: " << sph_shell->get_container_size() << vcl_endl;

#if 0
  TEST("# voxels of last layer with res min_res.. ", sph_shell->get_container_size(), 1026);
#endif

  vcl_vector<vgl_point_3d<double> >& cart_points = sph_shell->cart_points();
#if 0
  vcl_cout << "Cart points: ";
  for (vcl_vector<vgl_point_3d<double> >::const_iterator p = cart_points.begin(); p != cart_points.end(); ++p)
    vcl_cout << *p << vcl_endl;
#endif
  sph_shell->draw_template("./spherical_shell.vrml");
}

TESTMAIN(test_spherical_shell_container);
