#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <bbas/volm/volm_spherical_shell_container.h>
#include <bbas/volm/volm_spherical_shell_container_sptr.h>

static void test_spherical_shell_container()
{
  double cap_angle = 180;
  double point_angle = 10;
  double radius = 1;
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(radius, cap_angle, point_angle);

  vcl_cout << " for half spherical surface, point angle = " << point_angle << " degree, number of rays: " << sph_shell->get_container_size() << '\n'
           << " radius = " << sph_shell->radius() << " ,  cap_angle = " << sph_shell->cap_angle() << ", point_angle = " << sph_shell->point_angle()
           << " spherical center = " << sph_shell->cent() << '\n'
           << " for half spherical surface, point angle = " << point_angle << " degree, number of rays: " << sph_shell->get_container_size() << vcl_endl;

  TEST("# voxels of last layer with res min_res.. ", sph_shell->get_container_size(), 1026);

  vcl_vector<vgl_point_3d<double> >& cart_points = sph_shell->cart_points();
  vcl_cout << "Cart points: ";
  for (vcl_vector<vgl_point_3d<double> >::const_iterator p = cart_points.begin(); p != cart_points.end(); ++p)
    vcl_cout << *p << vcl_endl;
}

TESTMAIN(test_spherical_shell_container);
