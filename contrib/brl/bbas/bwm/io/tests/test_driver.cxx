#include <testlib/testlib_register.h>

DECLARE(test_kml_parser);
DECLARE(test_site_obj_reader);

void
register_tests()
{
  REGISTER(test_kml_parser);
  REGISTER(test_site_obj_reader);
}

DEFINE_MAIN;
