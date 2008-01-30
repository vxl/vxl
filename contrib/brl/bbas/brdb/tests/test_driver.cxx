#include <testlib/testlib_register.h>


DECLARE( test_value );
DECLARE( test_tuple );
DECLARE( test_relation );
DECLARE( test_database );
DECLARE( test_query );
//DECLARE( test_database_manager );

void
register_tests()
{
  REGISTER( test_value );
  REGISTER( test_tuple );
  REGISTER( test_relation );
  REGISTER( test_database );
  REGISTER( test_query );
//  REGISTER( test_database_manager );
}

DEFINE_MAIN;


