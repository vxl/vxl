// This is mul/mbl/tests/test_gamma.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_select_n_from_m.h>

void test_select_n_from_m()
{
  vcl_cout << "*******************\n"
           << " Testing mbl_select_n_from_m\n"
           << "*******************\n";

  mbl_select_n_from_m selector(2,9);
  
  selector.reset();
  TEST("Two selected",selector.subset().size(),2);
  TEST("First element 0",selector.subset()[0],0);
  TEST("Second element 1",selector.subset()[1],1);
  TEST("7 not selected",selector.complement().size(),7);

  int count=0;
  do
  {
    count++;
    vcl_cout<<selector.subset()[0]<<","<<selector.subset()[1]<<vcl_endl;
  }
  while (selector.next());
  vcl_cout<<"count = "<<count<<vcl_endl;

  TEST("2 from 9 = 9*8/2 = 36 combinations",count,36);

  selector.set_n_m(3,7);
  count=0;
  do { count++; } while (selector.next());
  TEST("3 from 7 = 7*6*5/(2*3) = 35 combinations",count,35);

  selector.set_n_m(1,6);
  count=0;
  do { count++; } while (selector.next());
  TEST("1 from 6 = 6 combinations",count,6);

  selector.set_n_m(6,6);
  count=0;
  do { count++; } while (selector.next());
  TEST("6 from 6 = one combination",count,1);

}

TESTLIB_DEFINE_MAIN(test_select_n_from_m);
