// This is mul/mbl/tests/test_jarque_bera.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <mbl/mbl_jarque_bera.h>
#include <vnl/vnl_random.h>

void test_jarque_bera()
{
  vcl_cout << "*************************\n"
           << " Testing mbl_jarque_bera\n"
           << "*************************\n";

  const unsigned n = 200;
  vnl_random rng(9667566ul);
  vcl_vector<double> x(n);
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
  vcl_cout << "mbl_jarque_bera (gauss_data)" << mbl_jarque_bera(x.begin(), x.end()) << vcl_endl;
  TEST_NEAR("mbl_jarque_bera (gauss pdf)", mbl_jarque_bera(x.begin(), x.end()),
    0.6100, 10-3);
}

TESTLIB_DEFINE_MAIN(test_gamma);
