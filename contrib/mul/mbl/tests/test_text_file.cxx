#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>

#include <testlib/testlib_test.h>

#include <vil/vil_print.h>

#include <vnl/vnl_math.h>

#include <mbl/mbl_save_text_file.h>
#include <mbl/mbl_load_text_file.h>


void test_text_file()
{
  vcl_cout << "****************************" << vcl_endl;
  vcl_cout << " Testing mbl_test_text_file " << vcl_endl;
  vcl_cout << "****************************" << vcl_endl;
	
  
  //create double vector
  vcl_vector<double> double_vec(3);
  double_vec[0]= -1.1;
  double_vec[1]= 4.0;
  double_vec[2]= -5.3;
   
  mbl_save_text_file( double_vec, "double_vec.txt");
  vcl_vector<double> double_vec_in;
  mbl_load_text_file( double_vec_in, "double_vec.txt");
  
  TEST( "double_vec io", double_vec==double_vec_in, true );
  
  //create int vector
  vcl_vector<int> int_vec(3);
  int_vec[0]= -1;
  int_vec[1]= 4;
  int_vec[2]= -5;
   
  mbl_save_text_file( int_vec, "int_vec.txt");
  vcl_vector<int> int_vec_in;
  mbl_load_text_file( int_vec_in, "int_vec.txt");
  
  TEST( "int_vec io", int_vec==int_vec_in, true );
 

  //create bool vector
  vcl_vector<bool> bool_vec(3);
  int_vec[0]= true;
  int_vec[1]= false;
  int_vec[2]= true;
   
  mbl_save_text_file( bool_vec, "bool_vec.txt");
  vcl_vector<bool> bool_vec_in;
  mbl_load_text_file( bool_vec_in, "bool_vec.txt");
  
  TEST( "bool_vec io", bool_vec==bool_vec_in, true );
 
  
 
}


TESTMAIN(test_text_file);
