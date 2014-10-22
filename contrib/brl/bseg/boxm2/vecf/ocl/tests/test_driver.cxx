#include <testlib/testlib_register.h>

DECLARE( test_transform );
DECLARE( test_store_nbrs );

void register_tests()
{
  REGISTER( test_transform );
  REGISTER( test_store_nbrs );
}

DEFINE_MAIN;
