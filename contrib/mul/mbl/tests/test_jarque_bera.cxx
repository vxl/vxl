// This is mul/mbl/tests/test_jarque_bera.cxx
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_jarque_bera.h>
#include <vnl/vnl_random.h>
#include <testlib/testlib_test.h>

void test_jarque_bera()
{
  std::cout << "*************************\n"
           << " Testing mbl_jarque_bera\n"
           << "*************************\n";

  constexpr unsigned n = 200;
  vnl_random rng(9667566ul);
  std::vector<double> x(n);
  for (unsigned i=0; i!=n; ++i)
  {
    x[i] = rng.drand32()*10 + 2;
  }

  TEST_NEAR("mbl_jarque_bera (flat pdf)", mbl_jarque_bera(x.begin(), x.end()),
    2.155e-4, 10-6);

  for (unsigned i=0; i!=n; ++i)
  {
    x[i] = rng.normal()*10 + 2;
  }
  std::cout << "mbl_jarque_bera (gauss_data)" << mbl_jarque_bera(x.begin(), x.end()) << std::endl;
  TEST_NEAR("mbl_jarque_bera (gauss pdf)", mbl_jarque_bera(x.begin(), x.end()),
    0.6100, 10-3);
}

TESTMAIN(test_jarque_bera);
