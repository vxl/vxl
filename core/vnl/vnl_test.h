// This is vxl/vnl/vnl_test.h
#ifndef vnl_test_h_
#define vnl_test_h_

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
// \brief Test functions for vnl.
// \author LGO 11/27/89 -- Initial design

#include <vcl_string.h>

//: initialise test counters, check test name 'name' exists
void vnl_test_start(const char* name);
//: increment number of tests, then output msg
void vnl_test_begin(const char* msg);
//: increment success/failure counters
void vnl_test_perform(int success);
//: output summary of tests performed
int  vnl_test_summary();

//: output msg, then perform test in expr
void vnl_test_assert(const vcl_string& msg, bool expr);
//: output msg, then perform test to see if expr is within tol of target
void vnl_test_assert_near(const vcl_string& msg, double expr,
                          double target = 0, double tol = 1e-12);

#if 0
#define Assert vnl_test_assert
#define AssertNear vnl_test_assert_near
#endif

//: initialise test
#define START(s) vnl_test_start(s);

//: TEST function, s is message, test to see if p==v
#define TEST(s,p,v) \
 { \
  vnl_test_begin(s); \
  vnl_test_perform((p)==(v)); \
 }

//: run x, s is message, then test to see if p==v
#define TEST_RUN(s,x,p,v) \
 { \
  x; \
  vnl_test_begin(s); \
  vnl_test_perform((p)==(v)); \
 }

//: summarise test
#define SUMMARY() vnl_test_summary();

//: main test program
#undef TESTMAIN
#define TESTMAIN(x) \
int main() { vnl_test_start(#x); x(); return vnl_test_summary(); }

// ---------------------------------------- handy for generating test data

#include <vcl_complex_fwd.h>
#define macro(T) void vnl_test_fill_random(T *begin, T *end)
macro(float);
macro(double);
macro(long double);
macro(vcl_complex<float>);
macro(vcl_complex<double>);
macro(vcl_complex<long double>);
#undef macro

#endif // vnl_test_h_
