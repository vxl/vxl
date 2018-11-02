// This is core/vul/tests/test_sleep_timer.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include <vpl/vpl.h>
#include <testlib/testlib_test.h>


void test_sleep_timer()
{
  vul_timer tic;
  vpl_sleep(1);
  double t = tic.real() / 1000.0;

  std::cout << "vul_timer: sleep lasted " << t << " seconds, expected 1.0\n";
  TEST_NEAR_REL("Sleep for between 0.4 and 2.5 seconds", t, 1.0, 0.6);
}

TEST_MAIN(test_sleep_timer);
