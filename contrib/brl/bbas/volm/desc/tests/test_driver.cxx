#include <testlib/testlib_register.h>


DECLARE( test_volm_descriptor );
DECLARE( test_ex_matcher );



void
register_tests()
{
  REGISTER( test_volm_descriptor );
  REGISTER( test_ex_matcher );
}

DEFINE_MAIN;


