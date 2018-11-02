// This is mul/mbl/tests/test_random_n_from_m.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_random_n_from_m.h>
#include <testlib/testlib_test.h>

void test_random_n_from_m()
{
  std::cout << "*****************************\n"
           << " Testing mbl_random_n_from_m\n"
           << "*****************************\n";

  mbl_random_n_from_m random;

  std::vector<int> chosen;
  random.choose_n_from_m(chosen,4,7);
  TEST("Four chosen",chosen.size(),4);
  for (int i : chosen)
  {
    TEST("Elements in [0,6]",i>=0 && i<7,true);
  }
  std::vector<int> not_chosen;
  random.choose_n_from_m(chosen,not_chosen,4,7);
  TEST("Three not chosen",not_chosen.size(),3);
  for (int i : not_chosen)
  {
    TEST("Elements in [0,6]",i>=0 && i<7,true);
  }
  random.choose_n_from_m(chosen,not_chosen,7,7);
  TEST("All chosen",not_chosen.size(),0);
  random.choose_n_from_m(chosen,0,7);
  TEST("None chosen",chosen.size(),0);
  random.choose_n_from_m(chosen,not_chosen,0,0);
  TEST("None chosen from none",chosen.size(),0);
  TEST("None not chosen from none",not_chosen.size(),0);
}

TESTMAIN(test_random_n_from_m);
