//:
// \file
// \author Ian Scott
// \brief test vpdfl_pc_gaussian, building, sampling, saving etc.

#include <vcl_iostream.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <mbl/mbl_selected_data_wrapper.h>
#include <testlib/testlib_test.h>

//=======================================================================

//: The main control program
void test_selected_data_wrapper()
{
  vcl_cout <<"\n\n***********************************\n"
           <<    " Testing mbl_selected_data_wrapper\n"
           <<    "***********************************\n";


  const unsigned n = 10;
  vcl_vector<double> data(n);

  for (unsigned i = 0; i < n; i++)
    data[i] = (double)i;

  mbl_data_array_wrapper<double > original(data);

  vcl_vector<unsigned> selector(2*n, 3);
  selector[0] = n-1;
  mbl_selected_data_wrapper<double> selection(original, selector);

  selection.reset();

  TEST ("Selected wrapper correct size", selection.size(), n*2);

  TEST ("First value correct", selection.current(), n-1);

  bool values_correct = true;
  selection.next();
  do
  {
    if (selection.current() != 3.0) values_correct=false;
  } while (selection.next());
  TEST ("Rest of values correct", values_correct, true);

  selection.set_index(0);
  TEST ("Set index correct", selection.current(), n-1);
}


TESTMAIN(test_selected_data_wrapper);
