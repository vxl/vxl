//:
// \file
// \brief vcl_tests.cxx : Run all vcl tests from one app.
//  I think this is preferable to having many vcl_test_* projects.
// \author awf, mar 2000

#include <vcl_iostream.h>
#include <vcl_string.h>

int test_algorithm_main();
int test_cctype_main();
int test_cmath_main();
int test_compiler_main();
int test_complex_main();
int test_deque_main();
int test_exception_main();
int test_fstream_main();
int test_headers_main(int, char**);
int test_iostream_main();
int test_iterator_main();
int test_list_main();
int test_map_main();
int test_multimap_main();
int test_new_main();
int test_rel_ops_main();
int test_set_main();
int test_stlfwd_main();
int test_string_main();
int test_vector_main();
int test_cstdio_main( const char* );

int result;

void testname( char* testname )
{
  vcl_cout << "   Testing " << testname << "...";
  vcl_cout.flush();
}

void testresult( int testresult )
{
  result |= testresult;
  if ( testresult==0 ) {
    vcl_cout << "    PASSED" << vcl_endl;
  } else {
    vcl_cout << "  **FAILED**" << vcl_endl;
  }
}

// The else is for a trailing ; after the macro
#define DO_TEST( Name, Func ) \
  if( name == "" || name == Name ) { \
    testname( Name ); \
    testresult( Func ); \
  } else

int main( int argc, char* argv[] )
{
  result = 0;
  vcl_string name;
 
  if( argc <= 1 )
    name = "";
  else {
    name = argv[1];
    ++argv;
    --argc;
  }

  
  DO_TEST( "test_algorithm", test_algorithm_main() );

  DO_TEST( "test_cctype",  test_cctype_main() );

  DO_TEST( "test_cmath", test_cmath_main() );

  DO_TEST( "test_compiler", test_compiler_main() );

  DO_TEST( "test_complex", test_complex_main() );

  DO_TEST( "test_deque", test_deque_main() );

  DO_TEST( "test_exception", test_exception_main() );

  DO_TEST( "test_fstream", test_fstream_main() );

  DO_TEST( "test_headers", test_headers_main(argc, argv) );

  DO_TEST( "test_iostream", test_iostream_main() );

  DO_TEST( "test_iterator", test_iterator_main() );

  DO_TEST( "test_list", test_list_main() );

  DO_TEST( "test_map", test_map_main() );

  DO_TEST( "test_multimap", test_multimap_main() );

  DO_TEST( "test_new", test_new_main() );

  DO_TEST( "test_set", test_set_main() );

  DO_TEST( "test_stlfwd", test_stlfwd_main() );

  DO_TEST( "test_string", test_string_main() );

  DO_TEST( "test_vector", test_vector_main() );

  if( name == "" || name == "test_cstdio" ) {
    testname( "test_cstdio" );
    if ( argc < 2 ) {
      vcl_cout << "No input file given for test_cstdio" << vcl_endl;
      testresult( 1 );
    } else {
      testresult( test_cstdio_main( argv[1] ) );
    }
  }

  return result;
}
