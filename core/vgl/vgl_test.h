#ifndef vgl_test_h_
#define vgl_test_h_

// This is vxl/vgl/vgl_test.h

//: \file
// \brief Testing software
// \author Tim Cootes

#include <vcl_string.h>

void vgl_test_start(const char* name);
void vgl_test_begin(const char* msg);
void vgl_test_perform(int success);
int  vgl_test_summary();

void vgl_test_assert(const vcl_string& msg, bool expr);
void vgl_test_assert_near(const vcl_string& msg, double expr, double target = 0, double tol = 1e-12);

#define Assert vgl_test_assert
#define AssertNear vgl_test_assert_near

#define START(s) vgl_test_start(s);

#define TEST(s,p,v)   \
 {  \
  vgl_test_begin(s);  \
  vgl_test_perform(p==v); \
 }

#define TEST_RUN(s,x,p,v) \
 {  \
  x \
  vgl_test_begin(s);  \
  vgl_test_perform(p==v); \
 }

#define SUMMARY() vgl_test_summary();

#define TESTMAIN(x) int main() { vgl_test_start(#x); x(); return vgl_test_summary(); }

#endif // vgl_test_h_
