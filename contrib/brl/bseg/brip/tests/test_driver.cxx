#include <testlib/testlib_register.h>


DECLARE( test_histogram );
DECLARE( test_mutual_info );
DECLARE( test_watershed );

void
register_tests()
{
  REGISTER( test_histogram );
  REGISTER( test_mutual_info );
  REGISTER(test_watershed );
}

DEFINE_MAIN;


