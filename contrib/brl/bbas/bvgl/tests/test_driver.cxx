#include <testlib/testlib_register.h>

DECLARE(test_bvgl_changes);
DECLARE(test_bvgl_volume_of_intersection);
DECLARE(test_bvgl_ray_pyramid);

void
register_tests()
{
  REGISTER(test_bvgl_changes);
  REGISTER(test_bvgl_volume_of_intersection);
  REGISTER(test_bvgl_ray_pyramid);
}

DEFINE_MAIN;
