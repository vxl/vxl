#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>

#include <mbl/mbl_gamma.h>

void test_gamma()
{
  vcl_cout << "******************************" << vcl_endl;
  vcl_cout << " Testing mbl_gamma " << vcl_endl;
  vcl_cout << "*****************************" << vcl_endl;

  TEST_NEAR("mbl_gamma_p(2,0)",      mbl_gamma_p(2,0),            0,  1e-8);
  TEST_NEAR("mbl_gamma_p(2,inf)",    mbl_gamma_p(2,1e9),        1.0,  1e-8);
  TEST_NEAR("mbl_gamma_q(2,0)",      mbl_gamma_q(2,0),          1.0,  1e-8);
  TEST_NEAR("mbl_gamma_q(2,inf)",    mbl_gamma_q(2,1e9),          0,  1e-8);
  TEST_NEAR("mbl_erf(0)",            mbl_erf(0),                  0,  1e-8);
  TEST_NEAR("mbl_erf(inf)",          mbl_erf(1e9),              1.0,  1e-8);
  TEST_NEAR("mbl_erf(-inf)",         mbl_erf(-1e9),            -1.0,  1e-8);
}

TESTLIB_DEFINE_MAIN(test_gamma);
