// This is mul/mbl/tests/test_gamma.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_random_n_from_m.h>

void test_random_n_from_m()
{
  vcl_cout << "*******************\n"
           << " Testing mbl_random_n_from_m\n"
           << "*******************\n";

  mbl_random_n_from_m random;

  vcl_vector<int> chosen;
  random.choose_n_from_m(chosen,4,7);
  TEST("Four chosen",chosen.size(),4);
  for (int i=0;i<chosen.size();++i)
  {
//    vcl_cout<<i<<") "<<chosen[i]<<vcl_endl;
    TEST("Element in [0,6]",chosen[i]>=0 && chosen[i]<7,true);
  }
}

TESTLIB_DEFINE_MAIN(test_random_n_from_m);
