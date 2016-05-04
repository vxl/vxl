#include <testlib/testlib_register.h>

DECLARE(test_geo_box);
DECLARE(test_geo_object);

void
register_tests()
{
  REGISTER(test_geo_box);
  REGISTER(test_geo_object);
}

DEFINE_MAIN;
