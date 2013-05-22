#include <testlib/testlib_register.h>


DECLARE( test_volm_descriptor );
DECLARE( test_volm_descriptor_query );


void
register_tests()
{
  REGISTER( test_volm_descriptor );
  REGISTER( test_volm_descriptor_query );
}

DEFINE_MAIN;


