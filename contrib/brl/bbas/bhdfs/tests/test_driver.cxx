#include <testlib/testlib_register.h>

DECLARE( test_manager );
DECLARE( test_hadoop );
DECLARE( test_fstream );
DECLARE( test_vil_stream );
DECLARE( test_vil_save_load_image );
void
register_tests()
{
  REGISTER( test_manager );
  REGISTER( test_hadoop );
  REGISTER( test_fstream );
  REGISTER( test_vil_stream );
  REGISTER( test_vil_save_load_image );
}

DEFINE_MAIN;
