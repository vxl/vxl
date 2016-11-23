#include <testlib/testlib_register.h>

DECLARE(test_geo_box);
DECLARE(test_geo_object);
DECLARE(test_site);
DECLARE(test_edgel_factory);
DECLARE(test_event_trigger);
DECLARE(test_edgel_change_detection);
DECLARE(test_json_params);

void
register_tests()
{
  REGISTER(test_geo_box);
  REGISTER(test_geo_object);
  REGISTER(test_site);
  REGISTER(test_edgel_factory);
  REGISTER(test_event_trigger);
  REGISTER(test_edgel_change_detection);
  REGISTER(test_json_params);
}

DEFINE_MAIN;
