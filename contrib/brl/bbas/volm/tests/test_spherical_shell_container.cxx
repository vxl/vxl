#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>
#include <bbas/volm/volm_spherical_shell_container.h>
#include <bbas/volm/volm_spherical_shell_container_sptr.h>

static void test_spherical_shell_container()
{
  float cap_angle = 180; // = 90;
  float point_angle = 10; // = 30;
  double radius = 1;
  float top_angle = 70;
  float bottom_angle = 50;
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(radius, cap_angle, point_angle, top_angle, bottom_angle);

  std::cout << " for half spherical surface, point angle = " << point_angle << " degree, number of rays: " << sph_shell->get_container_size() << '\n'
           << " radius = " << sph_shell->radius() << " ,  cap_angle = " << sph_shell->cap_angle() << ", point_angle = " << sph_shell->point_angle()
           << " spherical center = " << sph_shell->cent() << '\n'
           << " for half spherical surface, point angle = " << point_angle << " degree, number of rays: " << sph_shell->get_container_size() << std::endl;

#if 0
  TEST("# voxels of last layer with res min_res.. ", sph_shell->get_container_size(), 1026);
#endif

#if 0
  std::vector<vgl_point_3d<double> >& cart_points = sph_shell->cart_points();
  std::cout << "Cart points: ";
  for (std::vector<vgl_point_3d<double> >::const_iterator p = cart_points.begin(); p != cart_points.end(); ++p)
    std::cout << *p << std::endl;

  sph_shell->draw_template("./spherical_shell.vrml");

  vsl_b_ofstream os("./temp.bin");
  sph_shell->b_write(os);
  os.close();

  vsl_b_ifstream ifs("./temp.bin");
  volm_spherical_shell_container_sptr sp2 = new volm_spherical_shell_container;
  sp2->b_read(ifs);
  ifs.close();

  TEST("binary i/o", sph_shell->get_container_size(), sp2->get_container_size());
  TEST("binary i/o", *sph_shell == *sp2, true);
#endif
}

TESTMAIN(test_spherical_shell_container);
