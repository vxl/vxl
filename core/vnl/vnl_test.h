// This is a temporary vxl/vnl/vnl_test.h, to disappear soon.
#include <vcl_deprecated_header.h>
//
// Instead of including this file, include testlib/testlib_test.h,
// redefine MAIN as below, and replace all occurrences of
// "vnl_test_" by "testlib_test_".
//

#include <testlib/testlib_test.h>
#undef MAIN
#define MAIN( testname ) int main( int argc, char* argv[] )
#undef TESTMAIN
#define TESTMAIN( testname ) MAIN( testname ) { START( #testname ); testname(); SUMMARY(); }
#define vnl_test_start testlib_test_start
#define vnl_test_begin testlib_test_begin
#define vnl_test_perform testlib_test_perform
#define vnl_test_summary testlib_test_summary
