// This is vxl/vgl/vgl_test.h
#ifndef vgl_test_h_
#define vgl_test_h_

//: \file
// \brief Testing software
// \author Tim Cootes

#include <vcl_string.h>

//: initialise test counters, check test name 'name' exists
void vgl_test_start(const char* name);
//: increment number of tests, then output msg
void vgl_test_begin(const char* msg);
//: increment success/failure counters
void vgl_test_perform(int success);
//: output summary of tests performed
int  vgl_test_summary();

//: output msg, then perform test in expr
void vgl_test_assert(const vcl_string& msg, bool expr);
//: output msg, then perform test to see if expr is within tol of target
void vgl_test_assert_near(const vcl_string& msg, double expr,
                          double target = 0, double tol = 1e-12);

#define Assert vgl_test_assert
#define AssertNear vgl_test_assert_near

//: initialise test
#define START(s) vgl_test_start(s);

//: TEST function, s is message, test to see if p==v
#define TEST(s,p,v) \
 { \
  vgl_test_begin(s); \
  vgl_test_perform((p)==(v)); \
 }

//: run x, s is message, then test to see if p==v
#define TEST_RUN(s,x,p,v) \
 { \
  x; \
  vgl_test_begin(s); \
  vgl_test_perform((p)==(v)); \
 }

//: summarise test
#define SUMMARY() vgl_test_summary();

//: main test program
#undef TESTMAIN
#define TESTMAIN(x) \
int main() { vgl_test_start(#x); x(); return vgl_test_summary(); }

#endif // vgl_test_h_
