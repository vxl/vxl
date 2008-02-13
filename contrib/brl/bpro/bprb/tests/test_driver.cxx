#include <testlib/testlib_register.h>

DECLARE( test_process );
DECLARE( test_process_params );

void
register_tests()
{

  REGISTER( test_process );
  REGISTER( test_process_params );

}

DEFINE_MAIN;


