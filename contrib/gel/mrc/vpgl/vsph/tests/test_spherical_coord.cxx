#include <testlib/testlib_test.h>

#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_spherical_coord.h>
#include <vnl/vnl_math.h>

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
}

TESTMAIN(test_spherical_coord);