// This is vxl/vil/vil_test.h
#ifndef vil_test_h_
#define vil_test_h_

//:
// \file
// \brief Testing software
// \author Tim Cootes

#include <vcl_string.h>

//: initialise test counters, check test name 'name' exists
void vil_test_start(const char* name);
//: increment number of tests, then output msg
void vil_test_begin(const char* msg);
//: increment success/failure counters
void vil_test_perform(int success);
//: output summary of tests performed
int  vil_test_summary();

//: output msg, then perform test in expr
void vil_test_assert(const vcl_string& msg, bool expr);
//: output msg, then perform test to see if expr is within tol of target
void vil_test_assert_near(const vcl_string& msg, double expr,
                          double target = 0, double tol = 1e-12);

#define Assert vil_test_assert
#define AssertNear vil_test_assert_near

//: initialise test
#define START(s) vil_test_start(s);

//: TEST function, s is message, test to see if p==v
#define TEST(s,p,v) \
 { \
  vil_test_begin(s); \
  vil_test_perform((p)==(v)); \
 }

//: run x, s is message, then test to see if p==v
#define TEST_RUN(s,x,p,v) \
 { \
  x; \
  vil_test_begin(s); \
  vil_test_perform((p)==(v)); \
 }

//: summarise test
#define SUMMARY() vil_test_summary();

//: main test program
#undef TESTMAIN
#define TESTMAIN(x) \
int main() { vil_test_start(#x); x(); return vil_test_summary(); }

#endif // vil_test_h_
