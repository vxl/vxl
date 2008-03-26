#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>

#include <testlib/testlib_test.h>
//#include <geot/geot_cylinder_template.h>
//#include <util/util_image_functions.h>

#include <vil/vil_print.h>

#include <vnl/vnl_math.h>

#include <mbl/mbl_vector_txt_files.h>


void test_vector_txt_files()
{
  vcl_cout << "*******************************" << vcl_endl;
  vcl_cout << " Testing mbl_vector_txt_files " << vcl_endl;
  vcl_cout << "*******************************" << vcl_endl;
	
  /*
    void mbl_load_int_vec_vcl(vcl_vector<int>& v, const vcl_string& path);
    void mbl_save_int_vec_vcl(const vcl_vector<int>& v, const vcl_string& path);
  */
  
  //create int vector
  vcl_vector<int> int_vec(3);
  int_vec[0]= -1;
  int_vec[1]= 4;
  int_vec[2]= -5;
   
  mbl_save_int_vec_vcl( int_vec, "int_vec.txt");
  vcl_vector<int> int_vec_in;
  mbl_load_int_vec_vcl( int_vec_in, "int_vec.txt");
  
  TEST( "int_vec io", int_vec==int_vec_in, true );
 
  //create bool vector
  vcl_vector<bool> bool_vec(3);
  int_vec[0]= true;
  int_vec[1]= false;
  int_vec[2]= true;
   
  mbl_save_bool_vec_vcl( bool_vec, "bool_vec.txt");
  vcl_vector<bool> bool_vec_in;
  mbl_load_bool_vec_vcl( bool_vec_in, "bool_vec.txt");
  
  TEST( "bool_vec io", bool_vec==bool_vec_in, true );
 
  
  //test vcl_vector to string creation

  
 /*
  //: convert integer vector to a string
  // useful when saving to a text file
  // format  { n v1 v2 .. vn }
  void mbl_intvec2str(vcl_string& str, const vcl_vector<int>& v );


  //: convert string to a integer vector
  // useful when loading from a text file
  // format  { n v1 v2 .. vn }
  void mbl_str2intvec(vcl_vector<int>& v, const vcl_string& str);  
  */
  
  vcl_string int_str;
  mbl_intvec2str( int_str, int_vec );
  
  vcl_cout<<"int_str= "<<int_str<<vcl_endl;
  
  vcl_vector<int> int_vec_new;
  mbl_str2intvec( int_vec_new, int_str );
  
  int n= int_vec.size();
  for (int i=0; i<n; ++i)
  {
    vcl_cout<<"int_vec["<<i<<"]= "<<int_vec[i]<<vcl_endl;
    vcl_cout<<"int_vec_new["<<i<<"]= "<<int_vec_new[i]<<vcl_endl;
  }
  
  TEST( "int_vec string conversion", int_vec==int_vec_new, true );
  
  
  //create string vec
  vcl_vector<vcl_string> str_vec(3);
  str_vec[0]= "cat";
  str_vec[1]= "on";
  str_vec[2]= "mat";
  
  vcl_string str_str;
  mbl_strvec2str( str_str, str_vec );
  
  vcl_cout<<"str_str= "<<str_str<<vcl_endl;
  
  vcl_vector<vcl_string> str_vec_new;
  mbl_str2strvec( str_vec_new, str_str );
  
  int ns= str_vec.size();
  for (int i=0; i<ns; ++i)
  {
    vcl_cout<<"str_vec["<<i<<"]= "<<str_vec[i]<<vcl_endl;
    vcl_cout<<"str_vec_new["<<i<<"]= "<<str_vec_new[i]<<vcl_endl;
  }
  
  TEST( "str_vec string conversion", str_vec==str_vec_new, true );
  
}


TESTMAIN(test_vector_txt_files);
