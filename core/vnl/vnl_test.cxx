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
// Created: LGO 11/27/89 -- Initial design
//

#include <vcl/vcl_cstdio.h>
#include <vcl/vcl_iostream.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_test.h>

static int num_test;
static int tests_passed;
static int tests_failed;
static const char* test_name;

void vnl_test_start(const char* name = NULL) {
  num_test = 0;
  tests_passed = 0;
  tests_failed = 0;
  test_name = name;
  cout << "-----------------------------------------------------------------------------\n";
  cout << "Start Testing";	
  if (test_name != NULL) cout << " " << test_name;
  cout << ":\n-----------------------------------------------------------------------------\n";
  cout.flush();
 }

void vnl_test_begin(const char* msg) {
  num_test++;
#ifdef __GNUG__
  cout.form(" Test %03d: %-53s --> ", num_test, msg);
#else
  cout << " Test " << num_test << ": " << msg << " --> ";
#endif
  cout.flush();
}

// NOTE: We don't pass in the message (see test_begin) because
//       we want to ensure that the message is printed BEFORE
//       the test is executed.  This way when a test crashes
//       we can tell if it was during a test, or between tests.
void vnl_test_perform(int success) {
  if (success) {
    tests_passed++;
    cout << "  PASSED\n";
  } else {
    tests_failed++;
    cout << "**FAILED**\n";
  }
  cout.flush();
}

int vnl_test_summary() {
  cout << "-----------------------------------------------------------------------------\n";
  if (test_name != NULL) cout << test_name << " ";
  cout << "Test Summary: ";
  if (tests_failed > 0)
    cout<<tests_passed<<" tests succeeded, "<<tests_failed<<" tests didn't\t\t\t*****";
  else
    cout<<"All "<<tests_passed<<" tests succeeded";
  cout << "\n-----------------------------------------------------------------------------\n";
  cout.flush();
  return tests_failed;
}

void vnl_test_assert(const vcl_string& msg, bool expr)
{
  cout << msg << " - ";
  vnl_test_perform(expr);
}

void vnl_test_assert_near(const vcl_string& msg, double expr, double target, double tol)
{
  cout << msg << " should be " << target << ", is " << expr << ", ";
  vnl_test_perform(fabs(expr - target) < tol);
}
