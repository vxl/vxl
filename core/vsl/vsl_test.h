#ifndef vsl_test_h_
#define vsl_test_h_

//:
// \file
// \author Tim Cootes
// Testing software

#include <vcl_string.h>

void vsl_test_start(const char* name);
void vsl_test_begin(const char* msg);
void vsl_test_perform(int success);
int  vsl_test_summary();

void vsl_test_assert(const vcl_string& msg, bool expr);
void vsl_test_assert_near(const vcl_string& msg, double expr,
                           double target = 0, double tol = 1e-12);

#define Assert vsl_test_assert
#define AssertNear vsl_test_assert_near

#define START(s) vsl_test_start(s);

#define TEST(s,p,v) \
 {  \
  vsl_test_begin(s); \
  vsl_test_perform(p==v);  \
 }

#define TEST_RUN(s,x,p,v) \
 {  \
  x;  \
  vsl_test_begin(s); \
  vsl_test_perform(p==v);  \
 }

#define SUMMARY() vsl_test_summary();

#define TESTMAIN(x) int main()\
{ vsl_test_start(#x); x(); return vsl_test_summary(); }

#endif // vsl_test_h_
