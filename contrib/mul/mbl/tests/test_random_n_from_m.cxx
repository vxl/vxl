// This is mul/mbl/tests/test_random_n_from_m.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_random_n_from_m.h>
#include <testlib/testlib_test.h>

void test_random_n_from_m()
{
  vcl_cout << "*****************************\n"
           << " Testing mbl_random_n_from_m\n"
           << "*****************************\n";

  mbl_random_n_from_m random;

  vcl_vector<int> chosen;
  random.choose_n_from_m(chosen,4,7);
  TEST("Four chosen",chosen.size(),4);
  for (unsigned int i=0;i<chosen.size();++i)
  {
    TEST("Elements in [0,6]",chosen[i]>=0 && chosen[i]<7,true);
  }
  vcl_vector<int> not_chosen;
  random.choose_n_from_m(chosen,not_chosen,4,7);
  TEST("Three not chosen",not_chosen.size(),3);
  for (unsigned int i=0;i<not_chosen.size();++i)
  {
    TEST("Elements in [0,6]",not_chosen[i]>=0 && not_chosen[i]<7,true);
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
