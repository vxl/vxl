// This is mul/mbl/tests/test_stl.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_iterator.h>
#include <testlib/testlib_test.h>
#include <mbl/mbl_stl.h>

static void test_stl_sequence()
{
  vcl_cout << "**************************\n"
           << " Testing mbl_stl_sequence\n"
           << "**************************\n";

  const unsigned int n = 20;
  vcl_vector<double> x(n);

  for (unsigned int i=0;i<n;++i) x[i]=double(i);

  vcl_vector<double> y;

  mbl_stl_sequence_n(vcl_back_inserter(y), n,
    vcl_bind1st(vcl_plus<unsigned>(), 1u), 0u);

  TEST("Generate incremental sequence using mbl_stl_sequence_n", y, x);

  mbl_stl_increments(y.begin(), y.end(), 0);
  TEST("Generate incremental sequence using mbl_stl_increments", y, x);
}

void test_stl()
{
  test_stl_sequence();
}

TESTMAIN(test_stl);
