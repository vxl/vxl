#include <testlib/testlib_register.h>

DECLARE( test_arg );
DECLARE( test_file_iterator );
DECLARE( test_psfile );
DECLARE( test_sprintf );
DECLARE( test_url );
DECLARE( test_temp_filename );
DECLARE( test_regexp );
DECLARE( test_string );
DECLARE( test_sleep_timer );
DECLARE( test_vul_file );
DECLARE( test_get_time_as_string );

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
  REGISTER( test_sleep_timer );
  REGISTER( test_vul_file );
  REGISTER( test_get_time_as_string );
}

DEFINE_MAIN;
