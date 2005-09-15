// This is mul/mbl/tests/test_index_sort.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <testlib/testlib_test.h>
#include <mbl/mbl_stl.h>

static void test_stl_fold()
{
  vcl_cout << "***********************\n"
           << " Testing mbl_stl_fold* \n"
           << "***********************\n";

  const unsigned int n = 20;
  vcl_vector<double> x(n);

  for (unsigned int i=0;i<n;++i) x[i]=double(i);

  TEST("Correct result for foldl",
    mbl_stl_foldl(x.begin(), x.end(), vcl_plus<double>(), 0.5), ((n-1)*n/2)+0.5);
  TEST("Correct result for foldr",
    mbl_stl_foldr(x.begin(), x.end(), vcl_plus<double>(), 0.5), ((n-1)*n/2)+0.5);
}

void test_stl()
{
  test_stl_fold();
}

TESTMAIN(test_stl);
