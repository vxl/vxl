#ifndef TESTLIB_REGISTER_H_
#define TESTLIB_REGISTER_H_

//:
// \file
// \author Amitha Perera
// \brief  Macros for registering the tests with the driver.
// 
// A test driver program would simply look like
// \verbatim
//   #include <testlib/testlib_register.h>
//   DECLARE( some_test_name );
//   void register_tests()
//   {
//     REGISTER( some_test_name );
//   }
// \endverbatim
// The testlib library contains the main program to control the running of the tests.
// You will also have to link in a file defining a function
// \verbatim
//   int some_test_name_main(int,char*[])
// \endverbatim
// See the vxl tests for further examples (such as vil/tests).

#include <vcl_vector.h>
#include <vcl_string.h>

typedef int (*TestMainFunction)( int, char*[] );

extern vcl_vector<TestMainFunction> testlib_test_func_;
extern vcl_vector<vcl_string>       testlib_test_name_;

//: Declare the existence of the test.
// If you DECLARE( x ), then you will need to define a function int x_main(int,char*[]).
#define DECLARE( testname )  int testname ## _main ( int argc, char* argv[] )

//: Register the test with the driver.
// \param testname should be the same as one of the tests declared with DECLARE.
#define REGISTER( testname ) \
   testlib_test_func_.push_back( & testname ## _main ); \
   testlib_test_name_.push_back( #testname )

#endif
