// This is vxl/vul/vul_test.h
#ifndef vul_test_h_
#define vul_test_h_

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
// \brief Test functions for vul.
// \author LGO 11/27/89 -- Initial design

#include <vcl_string.h>

//: initialise test counters, check test name 'name' exists
void vul_test_start(const char* name);
//: increment number of tests, then output msg
void vul_test_begin(const char* msg);
//: increment success/failure counters
void vul_test_perform(int success);
//: output summary of tests performed
int  vul_test_summary();

//: output msg, then perform test in expr
void vul_test_assert(const vcl_string& msg, bool expr);
//: output msg, then perform test to see if expr is within tol of target
void vul_test_assert_near(const vcl_string& msg, double expr,
                          double target = 0, double tol = 1e-12);

#define Assert vul_test_assert
#define AssertNear vul_test_assert_near

//: initialise test
#define START(s) vul_test_start(s);

//: TEST function, s is message, test to see if p==v
#define TEST(s,p,v)   \
 {  \
  vul_test_begin(s);  \
  vul_test_perform(p==v); \
 }

//: run x, s is message, then test to see if p==v
#define TEST_RUN(s,x,p,v) \
 {  \
  x;\
  vul_test_begin(s);      \
  vul_test_perform(p==v); \
 }

//: summarise test
#define SUMMARY() vul_test_summary();

//: main test program
#define TESTMAIN(x) int main() \
{ vul_test_start(#x); x(); return vul_test_summary(); }

#endif // vul_test_h_
