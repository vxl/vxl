// This is mul/mbl/tests/test_combination.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_combination.h>
#include <testlib/testlib_test.h>

void test_combination()
{
  vcl_cout << "****************************\n"
           << " Testing mbl_combination_***\n"
           << "****************************\n";

  vcl_vector<unsigned> n(3);
  for (unsigned i=0;i<n.size();++i) n[i]=i+2;

  TEST("mbl_n_combinations",mbl_n_combinations(n),2*3*4);

  unsigned nc=0;
  vcl_vector<unsigned> x = mbl_combination_begin(n);
  do {nc++;}  while (mbl_combination_next(x,n));
  TEST("Loop count",nc,mbl_n_combinations(n));

  n[2]=1;
  x = mbl_combination_begin(n);
  for (int i=0;i<10;++i)
  {
    mbl_print_combination(vcl_cout,x); vcl_cout<<vcl_endl;
    mbl_combination_next(x,n);
  }
}

TESTMAIN(test_combination);
