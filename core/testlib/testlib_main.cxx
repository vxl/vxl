#include "testlib_register.h"

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

vcl_vector<TestMainFunction> testlib_test_func_;
vcl_vector<vcl_string>       testlib_test_name_;

void
list_test_names( vcl_ostream& ostr )
{
  ostr << "The registered test names are:\n";
  for( unsigned int i = 0; i < testlib_test_name_.size(); ++i ) {
    ostr << "   " << testlib_test_name_[i] << "\n";
  }
}

void
register_tests();

int
main( int argc, char* argv[] )
{
  register_tests();

  if( argc < 2 ) {
    vcl_cerr << "Test driver needs at least one parameter: the test name\n";
    list_test_names( vcl_cerr );
    return 1;
  }

  if( testlib_test_func_.size() != testlib_test_name_.size() ) {
    vcl_cerr << "Error: " << testlib_test_func_.size() << " test functions are registered, but "
             << testlib_test_name_.size() << " test names are registered." << vcl_endl;
    return 1;
  }

  for( unsigned int i = 0; i < testlib_test_name_.size(); ++i ) {
    if( testlib_test_name_[i] == argv[1] ) {
      return testlib_test_func_[i]( argc-1, argv+1 );
    }
  }

  vcl_cerr << "Test " << argv[1] << " not registered.\n";
  list_test_names( vcl_cerr );

  return 1;
}
