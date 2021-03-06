#include "testlib/testlib_register.h"


DECLARE(test_segmented_rolling_shutter_camera );
DECLARE(test_camera_utils );
DECLARE(test_geotif_camera );
DECLARE(test_lon_lat_camera );

void
register_tests()
{
  REGISTER(test_segmented_rolling_shutter_camera  );
  REGISTER(test_camera_utils  );
  REGISTER(test_geotif_camera  );
  REGISTER(test_lon_lat_camera  );
}

DEFINE_MAIN;
