//:
// \file
// \brief vcl_tests.cxx : Run all vcl tests from one app.
//  I think this is preferable to having many vcl_test_* projects.
// \author awf, mar 2000

#include <vcl_iostream.h>

int test_vcl_algorithm_main();
int test_vcl_cctype_main();
int test_vcl_cmath_main();
int test_vcl_compiler_main();
int test_vcl_complex_main();
int test_vcl_deque_main();
int test_vcl_exception_main();
int test_vcl_fstream_main();
int test_vcl_headers_main(int, char**);
int test_vcl_iostream_main();
int test_vcl_iterator_main();
int test_vcl_list_main();
int test_vcl_map_main();
int test_vcl_multimap_main();
int test_vcl_new_main();
int test_vcl_rel_ops_main();
int test_vcl_set_main();
int test_vcl_stlfwd_main();
int test_vcl_string_main();
int test_vcl_vector_main();

int result;

void testname( char* testname )
{
  vcl_cout << "   Testing " << testname << "...";
  vcl_cout.flush();
}

void testresult( int testresult )
{
  result |= testresult;
  if( testresult==0 ) {
    vcl_cout << "    PASSED" << vcl_endl;
  } else {
    vcl_cout << "  **FAILED**" << vcl_endl;
  }
}

int main(int argc, char* argv[])
{
  result = 0;

  testname( "test_vcl_algorithm" );
  testresult( test_vcl_algorithm_main() );

  testname( "test_vcl_cctype" );
  testresult( test_vcl_cctype_main() );

  testname( "test_vcl_cmath" );
  testresult( test_vcl_cmath_main() );

  testname( "test_vcl_compiler" );
  testresult( test_vcl_compiler_main() );

  testname( "test_vcl_complex" );
  testresult( test_vcl_complex_main() );

  testname( "test_vcl_deque" );
  testresult( test_vcl_deque_main() );

  testname( "test_vcl_exception" );
  testresult( test_vcl_exception_main() );

  testname( "test_vcl_fstream" );
  testresult( test_vcl_fstream_main() );

  testname( "test_vcl_headers" );
  testresult( test_vcl_headers_main(argc, argv) );

  testname( "test_vcl_iostream" );
  testresult( test_vcl_iostream_main() );

  testname( "test_vcl_iterator" );
  testresult( test_vcl_iterator_main() );

  testname( "test_vcl_list" );
  testresult( test_vcl_list_main() );

  testname( "test_vcl_map" );
  testresult( test_vcl_map_main() );

  testname( "test_vcl_multimap" );
  testresult( test_vcl_multimap_main() );

  testname( "test_vcl_new" );
  testresult( test_vcl_new_main() );

  testname( "test_vcl_set" );
  testresult( test_vcl_set_main() );

  testname( "test_vcl_stlfwd" );
  testresult( test_vcl_stlfwd_main() );

  testname( "test_vcl_string" );
  testresult( test_vcl_string_main() );

  testname( "test_vcl_vector" );
  testresult( test_vcl_vector_main() );

  return result;
}
