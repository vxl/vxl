// This is vxl/vbl/vbl_test.h
#ifndef vbl_test_h_
#define vbl_test_h_

// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.

//:
// \file
// \brief Test functions for vbl.
// \author LGO 11/27/89 -- Initial design

#include <vcl_string.h>

//: initialise test counters, check test name 'name' exists
void vbl_test_start(const char* name);
//: increment number of tests, then output msg
void vbl_test_begin(const char* msg);
//: increment success/failure counters
void vbl_test_perform(int success);
//: output summary of tests performed
int  vbl_test_summary();

//: output msg, then perform test in expr
void vbl_test_assert(const vcl_string& msg, bool expr);
//: output msg, then perform test to see if expr is within tol of target
void vbl_test_assert_near(const vcl_string& msg, double expr,
                          double target = 0, double tol = 1e-12);

#define Assert vbl_test_assert
#define AssertNear vbl_test_assert_near

//: initialise test
#define START(s) vbl_test_start(s);

//: TEST function, s is message, test to see if p==v
#define TEST(s,p,v) \
 { \
  vbl_test_begin(s); \
  vbl_test_perform((p)==(v)); \
 }

//: run x, s is message, then test to see if p==v
#define TEST_RUN(s,x,p,v) \
 { \
  x; \
  vbl_test_begin(s); \
  vbl_test_perform((p)==(v)); \
 }

//: summarise test
#define SUMMARY() vbl_test_summary();

//: main test program
#undef TESTMAIN
#define TESTMAIN(x) \
int main() { vbl_test_start(#x); x(); return vbl_test_summary(); }

#endif // vbl_test_h_
