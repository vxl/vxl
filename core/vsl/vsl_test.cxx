//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
// Created: 11-Mar-2001: TFC Copy of vsl_test
//

#include "vsl_test.h"
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h> // for setfill, setw (replaces cout.form())

static int num_test;
static int tests_passed;
static int tests_failed;
static const char* test_name;


//: A hack to allow programs in vsl/tests to use testlib
void register_tests(void)
{
}

void vsl_test_start(const char* name = 0) {
  num_test = 0;
  tests_passed = 0;
  tests_failed = 0;
  test_name = name;
  vcl_cout <<
    "--------------------------------------------------------------------\n";
  vcl_cout << "Start Testing";
  if (test_name != NULL) vcl_cout << " " << test_name;
  vcl_cout << ":\n" <<
    "--------------------------------------------------------------------\n";
  vcl_cout.flush();
 }

void vsl_test_begin(const char* msg) {
  num_test++;
#if 0 // .form() is non-ANSI // #if defined(__GNUG__) && !defined(GNU_LIBSTDCXX_V3)
  vcl_cout.form(" Test %03d: %-53s --> ", num_test, msg);
  vcl_cout.flush();
#else
  vcl_cout <<" Test "<< vcl_setw(3) << vcl_right << vcl_setfill('0') << num_test
           <<": "<< vcl_setw(53) << vcl_left << vcl_setfill(' ')<< msg <<" --> "
           << vcl_flush;
#endif
}

// NOTE: We don't pass in the message (see test_begin) because
//       we want to ensure that the message is printed BEFORE
//       the test is executed.  This way when a test crashes
//       we can tell if it was during a test, or between tests.
void vsl_test_perform(int success) {
  if (success) {
    tests_passed++;
    vcl_cout << "  PASSED\n" << vcl_flush;
  } else {
    tests_failed++;
    vcl_cout << "**FAILED**\n" << vcl_flush;
  }
}

int vsl_test_summary() {
  vcl_cout << 
    "--------------------------------------------------------------------\n";
  if (test_name != NULL) vcl_cout << test_name << " ";
  vcl_cout << "Test Summary: ";
  if (tests_failed > 0)
    vcl_cout<<tests_passed<<" tests succeeded, "<<tests_failed<<
      " tests failed\t\t\t*****";
  else
    vcl_cout<<"All "<<tests_passed<<" tests succeeded";
  vcl_cout << "\n" <<
    "--------------------------------------------------------------------\n";
  vcl_cout.flush();
  return tests_failed;
}

void vsl_test_assert(const vcl_string& msg, bool expr)
{
  vcl_cout << msg << " - ";
  vsl_test_perform(expr);
}

void vsl_test_assert_near(const vcl_string& msg,
                           double expr, double target, double tol)
{
  vcl_cout << msg << " should be " << target << ", is " << expr << ", ";
  vsl_test_perform(vcl_abs(expr - target) < tol);
}
