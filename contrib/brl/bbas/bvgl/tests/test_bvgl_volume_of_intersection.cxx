//:
// \file
#include <iostream>
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_volume_of_intersection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

//: Test changes
static void test_bvgl_volume_of_intersection()
{
  //test case two spheres no intersection
  vgl_sphere_3d<double> a0(0,0,0,2);
  vgl_sphere_3d<double> b0(3,0,0,1);
  double zeroVol = bvgl_volume_of_intersection(a0,b0);
  TEST_NEAR("Test bvgl_volume_of_intersection spheres (no intersection)", zeroVol, 0.0, 0.001);

  //test case one sphere inside the other
  vgl_sphere_3d<double> big(0,0,0,4);
  vgl_sphere_3d<double> small(0,1,1,1);
  double testVol = bvgl_volume_of_intersection(small,big);
  double smallVol = (4.0/3.0) * vnl_math::pi * small.radius()*small.radius()*small.radius();
  TEST_NEAR("Test bvgl_volume_of_intersection spheres (one inside the other)", testVol, smallVol, 0.001);

  //test case where spheres are same size and location
  vgl_sphere_3d<double> same0(1,2,3,4);
  vgl_sphere_3d<double> same1(1,2,3,4);
  double sameVol = bvgl_volume_of_intersection(same0,same1);
  double calcVol = (4.0/3.0) * vnl_math::pi * same1.radius()*same1.radius()*same1.radius();
  TEST_NEAR("Test bvgl_volume_of_intersection identical spheres", sameVol, calcVol, 0.001);

  //test case where distance = .69459 => vol of intersection is half of sphere's vol
  vgl_sphere_3d<double> half0(0,0,0,1);
  vgl_sphere_3d<double> half1(0,0.69459,0,1);
  double halfVol = bvgl_volume_of_intersection(half0,half1);
  double calcHalf = (4.0/6.0) * vnl_math::pi;
  TEST_NEAR("Test bvgl_volume_of_intersection (intersection volume should be exactly half) ", halfVol, calcHalf, 0.001);

  //test case where both sphere's centers are inside each other
  double r0 = 1.0, r1 = 5.0/8.0;
  double d = .5;
  vgl_sphere_3d<double> a1(0,0,0,r0);
  vgl_sphere_3d<double> b1(0,0,d,r1);
  double y = (r0*r0 - r1*r1 - d*d) / (2.0 * d);
  double h0 = r0 - (d + y);
  double h1 = r1 - y;
  double vCap0 = vnl_math::pi * h0*h0 * (3*r0 - h0) / 3.0;
  double vCap1 = vnl_math::pi * h1*h1 * (3*r1 - h1) / 3.0;
  double theorVol = (4.0/3.0)*vnl_math::pi*r1*r1*r1 - vCap1 + vCap0;
  double insideVol = bvgl_volume_of_intersection(a1,b1);
  TEST_NEAR("Test bvgl_volume_of_intersection centers are inside both spheres ", theorVol, insideVol, 0.001);

}

TESTMAIN( test_bvgl_volume_of_intersection );
