#include <testlib/testlib_register.h>

DECLARE( test_histogram );
DECLARE( test_mutual_info );
DECLARE( test_watershed );
DECLARE( test_fourier );
DECLARE( test_homography );
DECLARE( test_lucas_kanade );
DECLARE( test_Horn_Schunck );

void
register_tests()
{
  REGISTER( test_histogram );
  REGISTER( test_mutual_info );
  REGISTER( test_watershed );
  REGISTER( test_fourier );
  REGISTER( test_homography );
  REGISTER( test_lucas_kanade );
  REGISTER( test_Horn_Schunck );
}

DEFINE_MAIN;
