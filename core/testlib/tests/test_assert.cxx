#include <testlib/testlib_test.h>

void
test_it()
{
  testlib_test_assert( "Assert: ", true );
  testlib_test_assert_near( "Assert near: ", 1.2345, 1.2346, 0.001 );
}

MAIN( test_assert )
{
  RUN_TEST_FUNC( test_it );
}
