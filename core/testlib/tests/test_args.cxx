#include <testlib/testlib_test.h>

#include <vcl_string.h>

MAIN( test_args )
{
  START( "argument passing" );
  TEST( "argument count (should be 3) ", argc, 3 );
  if( argc == 3 ) {
    testlib_test_begin( "argv[0] should be the test name" );
    testlib_test_perform( vcl_string("test_args") == argv[0] );
    TEST( "argv[1] should be \"one\"", vcl_string("one"), argv[1] );
    TEST( "argv[2] should be \"two\"", vcl_string("two"), argv[2] );
  }
  SUMMARY();
}
