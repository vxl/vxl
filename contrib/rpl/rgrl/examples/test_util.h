#ifndef test_util_h_
#define test_util_h_
//:
// \file
#include <testlib/testlib_test.h>

//: To avoid critical error dialog boxes
void testlib_enter_stealth_mode();
void prepare_testing();

//:
// Perform testing
#define test_macro(description, result, accuracy) \
   { \
     START( description ); \
     testlib_test_perform( result < accuracy ); \
     SUMMARY(); \
   }

#endif
