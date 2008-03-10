#include <testlib/testlib_register.h>


//DECLARE( test_bil_scale_image );
DECLARE( test_bil_wshed2d );
DECLARE( test_bil_edt);
DECLARE( test_bil_cedt);
DECLARE( test_bil_finite_differences);
DECLARE( test_bil_finite_second_differences);
DECLARE( test_bil_detect_ridges);


void
register_tests()
{
 // REGISTER( test_bil_scale_image );
  REGISTER( test_bil_wshed2d );
  REGISTER( test_bil_edt );
  REGISTER( test_bil_cedt );
  REGISTER( test_bil_finite_differences);
  REGISTER( test_bil_finite_second_differences);
  REGISTER( test_bil_detect_ridges);
}

DEFINE_MAIN;
