#include <testlib/testlib_register.h>


DECLARE(test_segmented_rolling_shutter_camera );
DECLARE(test_camera_utils );

void
register_tests()
{
  REGISTER(test_segmented_rolling_shutter_camera  );
  REGISTER(test_camera_utils  );
}

DEFINE_MAIN;
