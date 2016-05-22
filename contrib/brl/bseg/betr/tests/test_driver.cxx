#include <testlib/testlib_register.h>

DECLARE(test_geo_box);
DECLARE(test_geo_object);
DECLARE(test_site);

void
register_tests()
{
  REGISTER(test_geo_box);
  REGISTER(test_geo_object);
  REGISTER(test_site);
}

DEFINE_MAIN;
