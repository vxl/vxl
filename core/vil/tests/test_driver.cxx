#include <testlib/testlib_register.h>

DECLARE( test_file_format_read );
DECLARE( test_save_load_image );


void
register_tests()
{
  REGISTER( test_save_load_image );
  REGISTER( test_file_format_read );
}

DEFINE_MAIN;
