// This is mul/mbl/tests/test_gamma.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_select_n_from_m.h>
#include <testlib/testlib_test.h>

void test_select_n_from_m()
{
  vcl_cout << "*****************************\n"
           << " Testing mbl_select_n_from_m\n"
           << "*****************************\n";

  mbl_select_n_from_m selector(2,9);
  
  selector.reset();
  TEST("Two selected",selector.subset().size(),2);
  TEST("First element 0",selector.subset()[0],0);
  TEST("Second element 1",selector.subset()[1],1);
  TEST("7 not selected",selector.complement().size(),7);
  TEST("First element 2",selector.complement()[0],2);
  TEST("Last element 8",selector.complement()[6],8);

  selector.next();
  TEST("Two selected",selector.subset().size(),2);
  TEST("First element 0",selector.subset()[0],0);
  TEST("Second element 2",selector.subset()[1],2);
  TEST("7 not selected",selector.complement().size(),7);
  TEST("First element 1",selector.complement()[0],1);
  TEST("Second element 3",selector.complement()[1],3);

  int count=0;
  if (selector.reset())
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
  if (selector.reset()) do { count++; } while (selector.next());
  TEST("3 from 7 = 7*6*5/(2*3) = 35 combinations",count,35);

  selector.set_n_m(1,6);
  count=0;
  if (selector.reset()) do { count++; } while (selector.next());
  TEST("1 from 6 = 6 combinations",count,6);

  selector.set_n_m(6,6);
  count=0;
  if (selector.reset()) do { count++; } while (selector.next());
  TEST("6 from 6 = one combination",count,1);

  selector.set_n_m(0,6);
  count=0;
  if (selector.reset()) do { count++; } while (selector.next());
  TEST("0 from 6 = one combination",count,1);

  selector.set_n_m(0,0);
  count=0;
  if (selector.reset()) do { count++; } while (selector.next());
  TEST("0 from 0 = one combination",count,1);

  selector.set_n_m(7,6);
  count=0;
  if (selector.reset()) do { count++; } while (selector.next());
  TEST("7 from 6 = no combinations",count,0);
}

TESTMAIN(test_select_n_from_m);
