// Some tests for vgl_ray_3d
// J.L. Mundy Sept. 17, 2010

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_closest_point.h>


static void test_constructor()
{
  vgl_vector_3d<double> t(0,0,2);
  vgl_point_3d<double> p(1,2,3);
  vgl_ray_3d<double> ray(p, t);
  vgl_point_3d<double> origin = ray.origin();
  vgl_vector_3d<double> dir = ray.direction();
  TEST_NEAR("Constructor from point and dir - compare origin", origin.x()+origin.y() , 3.0, 1e-5);
  TEST_NEAR("Constructor from point and dir - compare dir", dir.z_ ,1.0, 1e-5);
  vgl_point_3d<double> p1(1,2,4);
  vgl_ray_3d<double> ray1(p, p1);
  origin = ray1.origin();
  dir = ray1.direction();
  TEST_NEAR("Constructor from point-point - compare origin", origin.x()+origin.y() , 3.0, 1e-5);
  TEST_NEAR("Constructor from point-point - compare dir", dir.z_ ,1.0, 1e-5);

}

static void test_operations()
{
  vgl_vector_3d<double> t(0,0,1);
  vgl_point_3d<double> p(1,2,3), pt(1,2,2), clpt;
  vgl_ray_3d<double> ray(p, t);
  clpt = vgl_closest_point(ray, pt);
  bool con = ray.contains(clpt);
  TEST("Contains ", con, false);
}

void test_ray_3d()
{
  std::cout << "*****************************\n"
           << " Testing vgl_ray_3d\n"
           << "*****************************\n\n";

  test_constructor();
  test_operations();
}


TESTMAIN(test_ray_3d);
