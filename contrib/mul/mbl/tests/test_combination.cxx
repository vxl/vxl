// This is mul/mbl/tests/test_combination.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_combination.h>
#include <testlib/testlib_test.h>

void test_combination()
{
  std::cout << "****************************\n"
           << " Testing mbl_combination_***\n"
           << "****************************\n";

  std::vector<unsigned> n(3);
  for (unsigned i=0;i<n.size();++i) n[i]=i+2;

  TEST("mbl_n_combinations",mbl_n_combinations(n),2*3*4);

  unsigned nc=0;
  std::vector<unsigned> x = mbl_combination_begin(n);
  do {nc++;}  while (mbl_combination_next(x,n));
  TEST("Loop count",nc,mbl_n_combinations(n));

  n[2]=1;
  x = mbl_combination_begin(n);
  for (int i=0;i<10;++i)
  {
    mbl_print_combination(std::cout,x); std::cout<<std::endl;
    mbl_combination_next(x,n);
  }
}

TESTMAIN(test_combination);
