// This is mul/mbl/tests/test_gamma.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_gamma.h>
#include <testlib/testlib_test.h>

void test_gamma()
{
  std::cout << "*******************\n"
           << " Testing mbl_gamma\n"
           << "*******************\n";

  TEST_NEAR("mbl_gamma_p(2,0)",      mbl_gamma_p(2,0),            0,  1e-8);
  TEST_NEAR("mbl_gamma_p(2,inf)",    mbl_gamma_p(2,1e9),        1.0,  1e-8);
  TEST_NEAR("mbl_gamma_q(2,0)",      mbl_gamma_q(2,0),          1.0,  1e-8);
  TEST_NEAR("mbl_gamma_q(2,inf)",    mbl_gamma_q(2,1e9),          0,  1e-8);
  TEST_NEAR("mbl_erf(0)",            mbl_erf(0),                  0,  1e-8);
  TEST_NEAR("mbl_erf(inf)",          mbl_erf(1e9),              1.0,  1e-8);
  TEST_NEAR("mbl_erf(-inf)",         mbl_erf(-1e9),            -1.0,  1e-8);
}

TESTMAIN(test_gamma);
