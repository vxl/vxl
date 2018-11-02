//:
//\file
//\author Kongbin Kang
//\date Dec. 22th, 2004
//\brief test bnl_legendre_polynomial by comparing result with math table

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vnl/vnl_real_polynomial.h>
#include <bnl/algo/bnl_legendre_polynomial.h>

static void test_legendre_polynomial()
{
  for (int i = 0; i< 8; i++) {
    std::cout << " p"<< i <<" = ";
    vnl_real_polynomial p = bnl_legendre_polynomial(i);
    p.print(std::cout);
    std::cout << '\n';
  }
}

TESTMAIN(test_legendre_polynomial);
