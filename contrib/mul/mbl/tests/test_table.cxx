// This is mul/mbl/tests/test_table.cxx


//: \file
// \brief Test program for the mbl_table class
// \author Kevin de Souza
// \date 2005-02-02


#include <vcl_iostream.h>
#include <mbl/mbl_table.h>
#include <testlib/testlib_test.h>


//========================================================================
// Test the piecemeal construction of a table 
//========================================================================
void test_table1()
{
  vcl_cout << "------------------------------- \n"
           << " Testing piecemeal construction \n"
           << "------------------------------- \n";
   
  char delim = '\t';
  vcl_vector<vcl_string> headers(3);
  headers[0] = "x coord";
  headers[1] = "y coord";
  headers[2] = "z coord";

  mbl_table table(delim, headers);
  vcl_vector<double> row0(3);
  row0[0] = 1.23;
  row0[1] = 3.45;
  row0[2] = 6.78;
  table.append_row(row0);
  vcl_vector<double> row1(3);
  row1[0] = -1.23;
  row1[1] = -3.45;
  row1[2] = -6.78;
  table.append_row(row1);
  TEST("ncols==3", table.num_cols()==3, true);
  TEST("nrows==2", table.num_rows()==2, true);

  vcl_vector<double> col_vals;
  TEST("get_column(): success", table.get_column(headers[1], col_vals), true);
  TEST("get_column(): size", col_vals.size()==2, true);
  TEST("get_column(): values", (col_vals[0]==3.45 && col_vals[1]==-3.45), true);

  vcl_vector<double> row_vals;
  TEST("get_row(): success", table.get_row(1, row_vals), true);
  TEST("get_row(): size", row_vals.size()==3, true);
  TEST("get_row(): values", (row_vals[0]==-1.23 && 
                             row_vals[1]==-3.45 && 
                             row_vals[2]==-6.78), true);
}


//========================================================================
// Test equality operations 
//========================================================================
void test_table2()
{
  vcl_cout << "------------------------------- \n"
           << " Testing equality operations    \n"
           << "------------------------------- \n";
   
  char delim = '\t';
  vcl_vector<vcl_string> headers(3);
  headers[0] = "x coord";
  headers[1] = "y coord";
  headers[2] = "z coord";

  mbl_table table0(delim, headers);
  vcl_vector<double> row0(3);
  row0[0] = 1.23;
  row0[1] = 3.45;
  row0[2] = 6.78;
  table0.append_row(row0);
  vcl_vector<double> row1(3);
  row1[0] = -1.23;
  row1[1] = -3.45;
  row1[2] = -6.78;
  table0.append_row(row1);

  mbl_table table1 = table0;
  bool equal01 = table0 == table1;
  TEST("Copied tables equal", equal01, true);

  mbl_table table2(delim, headers);
  table2.append_row(row0);
  vcl_vector<double> row1_mod(3);
  row1_mod[0] = -1.23;
  row1_mod[1] = -3.45;
  row1_mod[2] = -6.78 + 1e-16;
  table2.append_row(row1_mod);
  bool equal02 = table0 == table2;
  TEST("Similar tables equal", equal02, true);

  mbl_table table3(delim, headers);
  table3.append_row(row0);
  vcl_vector<double> row1_mod2(3);
  row1_mod2[0] = -1.23;
  row1_mod2[1] = -3.45;
  row1_mod2[2] = -6.78 + 1e-14;
  table3.append_row(row1_mod2);
  bool equal03 = table0 == table3;
  TEST("Dissimilar tables not equal", equal03, false);

  mbl_table::set_tolerance(1e-12);
  equal03 = table0 == table3;
  TEST("Dissimilar tables equal within tolerance", equal03, true);
}


//========================================================================
// Test the get/set of existing elements of a table 
//========================================================================
void test_table3()
{
  vcl_cout << "------------------------------- \n"
           << " Testing get/set methods \n"
           << "------------------------------- \n";
  
  char delim = '\t';
  vcl_vector<vcl_string> headers(3);
  headers[0] = "x coord";
  headers[1] = "y coord";
  headers[2] = "z coord";
  
  mbl_table table(delim, headers);
  vcl_vector<double> row0(3);
  row0[0] = 1.23;
  row0[1] = 3.45;
  row0[2] = 6.78;
  table.append_row(row0);
  vcl_vector<double> row1(3);
  row1[0] = -1.23;
  row1[1] = -3.45;
  row1[2] = -6.78;
  table.append_row(row1);
  
  bool success=false;
  double val = table.get_element("y coord", 0, &success);
  TEST("Get existing element 1", (val==3.45 && success==true), true);

  val = table.get_element("z coord", 1, &success);
  TEST("Get existing element 2", (val==-6.78 && success==true), true);

  table.get_element("my column", 0, &success);
  TEST("Get element from non-existing column", success==false, true);

  table.get_element("x coord", 2, &success);
  TEST("Get element from non-existing row", success==false, true);


  success = table.set_element("y coord", 0, 9.99);
  TEST("Set existing element returns true", success==true, true);
  val = table.get_element("y coord", 0, &success);
  TEST("Set existing element", (val==9.99 && success==true), true);
  
  success = table.set_element("my column", 0, 1234);
  TEST("Set element in non-existing column returns false", success==false, true);

  success = table.set_element("x coord", 2, 1234);
  TEST("Set element in non-existing row returns false", success==false, true);
}



//========================================================================
// Run a series of tests
//========================================================================
void test_table()
{
  vcl_cout << "========================================\n"
           << " Testing mbl_table                      \n"
           << "========================================\n";
  
  mbl_table::set_verbosity(9);
  test_table1();

  test_table2();

  test_table3();
}



TESTMAIN(test_table);
