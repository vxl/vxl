// This is mul/mbl/tests/test_table.cxx


//: \file
// \brief Test program for the mbl_table class
// \author Kevin de Souza
// \date 2005-02-02


#include <vcl_iostream.h>
#include <mbl/mbl_table.h>
#include <testlib/testlib_test.h>



void test_table()
{
  vcl_cout << "***********************\n"
           << " Testing mbl_table     \n"
           << "***********************\n";

  TEST("Dummy test", true, true);
}


TESTMAIN(test_table);
