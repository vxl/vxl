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
  double theta=vnl_math::pi/4.0, phi=vnl_math::pi/4.0;
  vgl_point_3d<double> point = coord.cart_coord(theta,phi);

  // convert back to spherical
  vsph_sph_point_3d sp;
  coord.spherical_coord(point,sp);

  TEST_NEAR("radius", radius, sp.radius_, 1e-03);
  TEST_NEAR("theta", theta, sp.theta_, 1e-03);
  TEST_NEAR("phi", phi, sp.phi_, 1e-03);

  // test sp(2,0.2,1.0) --> cart(0.2147, 0.3343, 1.960)
  vsph_sph_point_3d sp1(2,0.2,1.0);
  vgl_point_3d<double> test_p1(0.2147, 0.3343, 1.960);
  point=sp1.cart_coord();
  double diff = vgl_distance(point, test_p1);
  TEST_NEAR("Conversion 1 ", diff, 0.0, 0.01);

  // test sp(2,-0.2,1.0) --> cart(-0.2147, -0.3343, 1.960)
  vsph_sph_point_3d sp2(2,-0.2,1.0);
  vgl_point_3d<double> test_p2(-0.2147, -0.3343, 1.960);
  point=sp2.cart_coord();
  diff = vgl_distance(point, test_p2);
  TEST_NEAR("Conversion 2 ", diff, 0.0, 0.01);

  // test cart(10,0.5,0.7)-->sp(10.04, 1.501, 0.04996)
  vgl_point_3d<double> test_p3(10,0.5,0.7);
  vsph_sph_point_3d sp3;
  coord.spherical_coord(test_p3, sp3);
  diff = vgl_distance(vgl_point_3d<double>(sp3.radius_,sp3.theta_,sp3.phi_), vgl_point_3d<double>(10.04, 1.501, 0.04996));
  TEST_NEAR("Reverse Conversion 1 ", diff, 0.0, 0.01);

  // test cart(2, 2, 2)-->sp(3.464, 0.9553, 0.7854)
  vgl_point_3d<double> test_p4(2, 2, 2);
  vsph_sph_point_3d sp4;
  coord.spherical_coord(test_p4, sp4);
  diff = vgl_distance(vgl_point_3d<double>(sp4.radius_,sp4.theta_,sp4.phi_), vgl_point_3d<double>(3.464, 0.9553, 0.7854));
  TEST_NEAR("Reverse Conversion 2 ", diff, 0.0, 0.01);
}

TESTMAIN(test_spherical_coord);
