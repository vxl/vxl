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

  TEST("mbl_gamma_p(2,0)==0", vcl_fabs(mbl_gamma_p(2,0))<1e-8, true);
  TEST("mbl_gamma_p(2,inf)==1", vcl_fabs(mbl_gamma_p(2,1e9)-1)<1e-8, true);
  TEST("mbl_gamma_q(2,0)==1", vcl_fabs(mbl_gamma_q(2,0)-1)<1e-8, true);
  TEST("mbl_gamma_q(2,inf)==0", vcl_fabs(mbl_gamma_q(2,1e9))<1e-8, true);
  TEST("mbl_erf(0)==0", vcl_fabs(mbl_erf(0))<1e-8, true);
  TEST("mbl_erf(inf)==1", vcl_fabs(mbl_erf(1e9)-1)<1e-8, true);
  TEST("mbl_erf(-inf)==-1", vcl_fabs(mbl_erf(-1e9)+1)<1e-8, true);
}

TESTLIB_DEFINE_MAIN(test_gamma);
