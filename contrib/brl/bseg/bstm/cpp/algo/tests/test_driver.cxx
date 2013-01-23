#include <testlib/testlib_register.h>


DECLARE( test_time_tree_ingestion);
DECLARE( test_boxm2_ingestion);

void register_tests()
{
  REGISTER( test_time_tree_ingestion );
  REGISTER( test_boxm2_ingestion);

}

DEFINE_MAIN;
