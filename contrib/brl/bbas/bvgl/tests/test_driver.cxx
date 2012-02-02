#include <testlib/testlib_register.h>

DECLARE(test_bvgl_changes);
DECLARE(test_bvgl_volume_of_intersection);
DECLARE(test_bvgl_ray_pyramid);
DECLARE(test_bvgl_intersection);

void
register_tests()
{
  REGISTER(test_bvgl_changes);
  REGISTER(test_bvgl_volume_of_intersection);
  REGISTER(test_bvgl_ray_pyramid);
  REGISTER(test_bvgl_intersection);
}

DEFINE_MAIN;
