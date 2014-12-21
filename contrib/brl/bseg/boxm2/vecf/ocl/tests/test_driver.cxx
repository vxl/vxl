#include <testlib/testlib_register.h>

DECLARE( test_transform );
DECLARE( test_store_nbrs );
DECLARE( test_filter );


void register_tests()
{
  REGISTER( test_transform );
  REGISTER( test_store_nbrs );
  REGISTER( test_filter );

}

DEFINE_MAIN;
