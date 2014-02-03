// Some tests for vgl_frustum_3d
// J.L. Mundy December. 1, 2013

#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_frustum_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_box_3d.h>


static void test_all()
{
  vgl_point_3d<double> apex(0.0, 0.0, 10.0);
  vgl_point_3d<double> p0(10.0, 10.0, 0.0);
  vgl_point_3d<double> p1(-10.0, 10.0, 0.0);
  vgl_point_3d<double> p2(-10.0, -10.0, 0.0);
  vgl_point_3d<double> p3(10.0, -10.0, 0.0);
  vgl_vector_3d<double> dir0 = p0-apex;
  vgl_vector_3d<double> dir1 = p1-apex;
  vgl_vector_3d<double> dir2 = p2-apex;
  vgl_vector_3d<double> dir3 = p3-apex;
  vgl_ray_3d<double> r0(apex, dir0);
  vgl_ray_3d<double> r1(apex, dir1);
  vgl_ray_3d<double> r2(apex, dir2);
  vgl_ray_3d<double> r3(apex, dir3);
  double d0 = 5.0, d1 = 10.0;
  vgl_vector_3d<double> norm(0.0, 0.0, 1.0);
  vcl_vector<vgl_ray_3d<double> > rays;
  rays.push_back(r0);  rays.push_back(r1);
  rays.push_back(r2);  rays.push_back(r3);
  vgl_frustum_3d<double> f(rays, norm, d0, d1);
  vcl_cout << f;
  bool in = f.contains(0.0, 0.0, 2.5);
  bool not_in = !f.contains(10.0, 10.000001, 0.0);
  TEST("Frustum contains a point", in&&not_in, true);
  vgl_box_3d<double> b = f.bounding_box();
  vgl_box_3d<double> b_test;
  vgl_point_3d<double> b0(-10, -10, 0), b1(10, 10, 5);
  b_test.add(b0); b_test.add(b1);
  bool bgood = b == b_test;
  vgl_point_3d<double> cent = f.centroid();
  bgood = bgood && (cent == vgl_point_3d<double>(0.0, 0.0, 2.5));
  TEST("Frustum bounding box and centroid", bgood,  true);
  bool conv = f.is_convex();
}

void test_frustum_3d()
{
  vcl_cout << "*****************************\n"
           << " Testing vgl_frustum_3d\n"
           << "*****************************\n\n";

  test_all();
}


TESTMAIN(test_frustum_3d);
