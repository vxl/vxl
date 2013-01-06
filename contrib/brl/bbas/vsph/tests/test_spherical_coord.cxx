#include <testlib/testlib_test.h>

#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_spherical_coord.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>

static void test_spherical_coord()
{
  // define the spherical coordinate system
  vgl_point_3d<double> origin(0,0,0);
  double radius = 10.0;
  vsph_spherical_coord coord(origin, radius);

  // test the conversions
  // convert to cartesian coordinates
  double theta=vnl_math::pi_over_4, phi=vnl_math::pi_over_4;
  vgl_point_3d<double> point = coord.cart_coord(theta,phi);

  // convert back to spherical
  vsph_sph_point_3d sp;
  coord.spherical_coord(point,sp);

  TEST_NEAR("radius", radius, sp.radius_, 1e-9);
  TEST_NEAR("theta", theta, sp.theta_, 1e-9);
  TEST_NEAR("phi", phi, sp.phi_, 1e-9);

  // test sp(2,0.2,1.0) --> cart(0.2147, 0.3343, 1.960)
  vsph_sph_point_3d sp1(2,0.2,1.0); // note: radius 2, not 10
  vgl_point_3d<double> test_p1(0.2146829950677, 0.3343489548705, 1.9601331556825);
  point=coord.cart_coord(sp1); // which forces the radius to 2 instead of 10
  double diff = vgl_distance(point, test_p1);
  TEST_NEAR("Conversion 1", diff, 0.0, 1e-9);

  // test sp(2,-0.2,1.0) --> cart(-0.2147, -0.3343, 1.960)
  vsph_sph_point_3d sp2(2,-0.2,1.0);
  vgl_point_3d<double> test_p2(-0.2146829950677, -0.3343489548705, 1.9601331556825);
  point=coord.cart_coord(sp2);
  diff = vgl_distance(point, test_p2);
  TEST_NEAR("Conversion 2", diff, 0.0, 1e-9);

  // test cart(10,0.5,0.7)-->sp(10.04, 1.501, 0.04996)
  vgl_point_3d<double> test_p3(10,0.5,0.7);
  vsph_sph_point_3d sp3;
  point.set(10.03693180209968, 1.500997236106283, 0.0499583957219428);
  coord.spherical_coord(test_p3, sp3);
  diff = vgl_distance(vgl_point_3d<double>(sp3.radius_,sp3.theta_,sp3.phi_), point);
  TEST_NEAR("Reverse Conversion 1", diff, 0.0, 1e-9);

  // test cart(2, 2, 2)-->sp(3.464, 0.9553, 0.7854)
  vgl_point_3d<double> test_p4(2, 2, 2);
  vsph_sph_point_3d sp4;
  point.set(3.464101615137754, 0.9553166181245092, vnl_math::pi_over_4);
  coord.spherical_coord(test_p4, sp4);
  diff = vgl_distance(vgl_point_3d<double>(sp4.radius_,sp4.theta_,sp4.phi_), point);
  TEST_NEAR("Reverse Conversion 2", diff, 0.0, 1e-9);
}

TESTMAIN(test_spherical_coord);
