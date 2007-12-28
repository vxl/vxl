#include <testlib/testlib_register.h>


DECLARE(test_cam_iostream);
DECLARE(test_video_site_io);

  
void
register_tests()
{
  REGISTER(test_cam_iostream);
  REGISTER(test_video_site_io);
}

DEFINE_MAIN;
