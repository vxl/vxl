#include <testlib/testlib_register.h>

DECLARE(test_meter);
DECLARE(test_radian);
DECLARE(test_degree);
DECLARE(test_cartesian_2d);
DECLARE(test_cartesian_3d);
DECLARE(test_polar);
DECLARE(test_cylindrical);
DECLARE(test_spherical);
DECLARE(test_transformation_graph);
DECLARE(test_composition);
DECLARE(test_scale);
DECLARE(test_translation);
DECLARE(test_rotation);
DECLARE(test_displacement);

void
register_tests()
{
  REGISTER(test_meter);
  REGISTER(test_radian);
  REGISTER(test_degree);
  REGISTER(test_cartesian_2d);
  REGISTER(test_cartesian_3d);
  REGISTER(test_polar);
  REGISTER(test_cylindrical);
  REGISTER(test_spherical);
  REGISTER(test_transformation_graph);
  REGISTER(test_composition);
  REGISTER(test_scale);
  REGISTER(test_translation);
  REGISTER(test_rotation);
  REGISTER(test_displacement);
}

DEFINE_MAIN;
