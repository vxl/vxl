// This is vxl/vsl/vsl_test.h
#ifndef vsl_test_h_
#define vsl_test_h_

//:
// \file
// \author Tim Cootes
// Testing software

#include <vcl_string.h>

//: initialise test counters, check test name 'name' exists
void vsl_test_start(const char* name);
//: increment number of tests, then output msg
void vsl_test_begin(const char* msg);
//: increment success/failure counters
void vsl_test_perform(int success);
//: output summary of tests performed
int  vsl_test_summary();

//: output msg, then perform test in expr
void vsl_test_assert(const vcl_string& msg, bool expr);
//: output msg, then perform test to see if expr is within tol of target
void vsl_test_assert_near(const vcl_string& msg, double expr,
                          double target = 0, double tol = 1e-12);

#define Assert vsl_test_assert
#define AssertNear vsl_test_assert_near

//: initialise test
#define START(s) vsl_test_start(s);

//: TEST function, s is message, test to see if p==v
#define TEST(s,p,v) \
 { \
  vsl_test_begin(s); \
  vsl_test_perform((p)==(v)); \
 }

//: run x, s is message, then test to see if p==v
#define TEST_RUN(s,x,p,v) \
 { \
  x; \
  vsl_test_begin(s); \
  vsl_test_perform((p)==(v)); \
 }

//: summarise test
#define SUMMARY() vsl_test_summary();

//: main test program
#undef TESTMAIN
#define TESTMAIN(x) \
int main() { vsl_test_start(#x); x(); return vsl_test_summary(); }

#endif // vsl_test_h_
