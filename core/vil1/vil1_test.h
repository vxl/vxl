#ifndef vil_test_h_
#define vil_test_h_

// This is vxl/vil/vil_test.h

//:
// \file
// \brief Testing software
// \author Tim Cootes

#include <vcl_string.h>

void vil_test_start(const char* name);
void vil_test_begin(const char* msg);
void vil_test_perform(int success);
int  vil_test_summary();

void vil_test_assert(const vcl_string& msg, bool expr);
void vil_test_assert_near(const vcl_string& msg, double expr, double target = 0, double tol = 1e-12);

#define Assert vil_test_assert
#define AssertNear vil_test_assert_near

#define START(s) vil_test_start(s);

#define TEST(s,p,v)   \
 {  \
  vil_test_begin(s);  \
  vil_test_perform(p==v); \
 }

#define TEST_RUN(s,x,p,v) \
 {  \
  x;\
  vil_test_begin(s);  \
  vil_test_perform(p==v); \
 }

#define SUMMARY() vil_test_summary();

#define TESTMAIN(x) int main() { vil_test_start(#x); x(); return vil_test_summary(); }

#endif // vil_test_h_
