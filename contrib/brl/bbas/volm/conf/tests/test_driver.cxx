#include <testlib/testlib_register.h>

DECLARE( test_volm_conf_object );
DECLARE( test_volm_conf_query  );

void
register_tests()
{
  REGISTER( test_volm_conf_object );
  REGISTER( test_volm_conf_query  );
}

DEFINE_MAIN;