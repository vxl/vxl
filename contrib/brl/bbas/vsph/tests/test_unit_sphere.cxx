#include <testlib/testlib_test.h>

#include <vsph/vsph_unit_sphere.h>
#include <vsph/vsph_sph_point_2d.h>
#include <vsph/vsph_sph_box_2d.h>
#include <vsph/vsph_utils.h>

#include <vnl/vnl_math.h>


static void test_unit_sphere()
{
  float cap_angle = 180.0f;
  float point_angle = 5.0f;//for coast (2 really)
  double radius = 1;
  float top_angle = 70.0f;
  float bottom_angle = 60.0f;
  vsph_unit_sphere usph;
  usph.add_uniform_views(cap_angle, point_angle);
}
TESTMAIN(test_unit_sphere);

