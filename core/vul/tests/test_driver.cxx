#include <testlib/testlib_register.h>

DECLARE( test_arg );
DECLARE( test_file_iterator );
DECLARE( test_psfile );
DECLARE( test_sprintf );
DECLARE( test_url );
DECLARE( test_temp_filename );
DECLARE( test_regexp );
DECLARE( test_string );

void
register_tests()
{
  REGISTER( test_arg );
  REGISTER( test_file_iterator );
  REGISTER( test_psfile );
  REGISTER( test_sprintf );
  REGISTER( test_url );
  REGISTER( test_temp_filename );
  REGISTER( test_regexp );
  REGISTER( test_string );
}

DEFINE_MAIN;
